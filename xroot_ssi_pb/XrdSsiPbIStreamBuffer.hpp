/*!
 * @project        The CERN Tape Archive (CTA)
 * @brief          Input stream to receive a stream of protocol buffers from the server
 * @copyright      Copyright 2017 CERN
 * @license        This program is free software: you can redistribute it and/or modify
 *                 it under the terms of the GNU General Public License as published by
 *                 the Free Software Foundation, either version 3 of the License, or
 *                 (at your option) any later version.
 *
 *                 This program is distributed in the hope that it will be useful,
 *                 but WITHOUT ANY WARRANTY; without even the implied warranty of
 *                 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *                 GNU General Public License for more details.
 *
 *                 You should have received a copy of the GNU General Public License
 *                 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <google/protobuf/io/coded_stream.h>

#include "XrdSsiPbException.hpp"

namespace XrdSsiPb {

/*!
 * Input Stream Buffer class
 *
 * This implementation is for a record-based stream, i.e. the client must be configured with a XrdSsi stream buffer size
 * which is large with respect to the maximum size of DataType. This is mainly for efficiency reasons as there is a
 * little extra overhead in handling records which are split across two SSI buffers. There is a hard limit that the
 * record size cannot exceed the buffer size.
 *
 * The buffer size parameter is set in the constructor to XrdSsiPbServiceClientSide. The size of DataType is the maximum
 * encoded size of the DataType protocol buffer on the wire.
 *
 * If there is a requirement to stream arbitrarily large binary blobs rather than records, this functionality needs to
 * be added. See the comments on Request::ProcessResponseData().
 */
template<typename DataType>
class IStreamBuffer
{
public:
   IStreamBuffer(uint32_t bufsize) :
      m_max_msglen(bufsize-sizeof(uint32_t)),
      m_split_buflen(0)
   {
#ifdef XRDSSI_DEBUG
      std::cerr << "[DEBUG] IStreamBuffer() constructor" << std::endl;
#endif
      m_split_buffer = std::unique_ptr<uint8_t[]>(new uint8_t[m_max_msglen]);
   }

   ~IStreamBuffer() {
#ifdef XRDSSI_DEBUG
      std::cerr << "[DEBUG] IStreamBuffer() destructor" << std::endl;
#endif
   }

   /*!
    * Push a new buffer onto the input stream
    *
    * NOTE: This method is not reentrant; it is assumed it will be called from the XrdSsi framework
    *       in single-threaded mode. Each client or client thread must set up its own stream.
    */
   void push(const char *buf_ptr, int buf_len);

private:
   /*!
    * Pop a single record of known size from an input stream and pass it to the client
    */
   bool popRecord(int msg_len, google::protobuf::io::CodedInputStream &input_stream);

   // Member variables

   const uint32_t             m_max_msglen;      //!< Maximum allowed length of a protobuf on the wire
   std::unique_ptr<uint8_t[]> m_split_buffer;    //!< Holding buffer for partial messages split across two input buffers
   int                        m_split_buflen;    //!< Length of data stored in m_split_buffer
};



template<typename DataType>
void IStreamBuffer<DataType>::push(const char *buf_ptr, int buf_len)
{
   google::protobuf::io::CodedInputStream input_stream(reinterpret_cast<const uint8_t*>(buf_ptr), buf_len);

   uint32_t msg_len;

   if(m_split_buflen > 0) {
      // Stitch together the partial record

      if(m_split_buflen <= sizeof(uint32_t)) {
         // The size field is split across the boundary, only need to copy that field

         int bytes_to_copy = sizeof(uint32_t) - m_split_buflen;
std::cout << "BEFORE <"  << static_cast<int>(m_split_buffer.get()[0])
          << "><" << static_cast<int>(m_split_buffer.get()[1])
          << "><" << static_cast<int>(m_split_buffer.get()[2])
          << "><" << static_cast<int>(m_split_buffer.get()[3]) << ">" << std::endl;
         memcpy(m_split_buffer.get() + m_split_buflen, buf_ptr, bytes_to_copy);
         input_stream.Skip(bytes_to_copy);
std::cout << "AFTER <"  << static_cast<int>(m_split_buffer.get()[0])
          << "><" << static_cast<int>(m_split_buffer.get()[1])
          << "><" << static_cast<int>(m_split_buffer.get()[2])
          << "><" << static_cast<int>(m_split_buffer.get()[3]) << ">" << std::endl;

         google::protobuf::io::CodedInputStream::ReadLittleEndian32FromArray(m_split_buffer.get(), &msg_len);
std::cout << "Option 1, msg_len = " << msg_len << std::endl;
         popRecord(msg_len, input_stream);
      } else {
         // The payload is split across the boundary, copy the rest of the record and extract it

         google::protobuf::io::CodedInputStream::ReadLittleEndian32FromArray(m_split_buffer.get(), &msg_len);
         if(msg_len > m_max_msglen) {
            throw XrdSsiException("Data record size exceeds XRootD SSI buffer size");
         }
         int bytes_to_copy = msg_len + sizeof(uint32_t) - m_split_buflen;
         memcpy(m_split_buffer.get() + m_split_buflen, buf_ptr, bytes_to_copy);
         input_stream.Skip(bytes_to_copy);

         google::protobuf::io::CodedInputStream split_stream(reinterpret_cast<const uint8_t*>(m_split_buffer.get() + sizeof(uint32_t)), msg_len);
std::cout << "Option 2, msg_len = " << msg_len << std::endl;
         popRecord(msg_len, split_stream);
      }
   }

   // Extract remaining records from the input buffer

   do {
      const char *buf_ptr;

      if(!input_stream.GetDirectBufferPointer(reinterpret_cast<const void**>(&buf_ptr), &buf_len)) break;
std::cout << "[POP_RECORDS] buf_len = " << buf_len << std::endl;

      if(buf_len < static_cast<int>(sizeof(uint32_t))) {
         // Size field is split across the boundary, save the partial field and finish
std::cout << "[POP_RECORDS] Saving split of " << buf_len << " bytes" << std::endl;
         m_split_buflen = buf_len;
std::cout << "SAVING/BEFORE <"  << static_cast<int>(m_split_buffer.get()[0])
          << "><" << static_cast<int>(m_split_buffer.get()[1])
          << "><" << static_cast<int>(m_split_buffer.get()[2])
          << "><" << static_cast<int>(m_split_buffer.get()[3]) << ">" << std::endl;
         memcpy(m_split_buffer.get(), buf_ptr, m_split_buflen);
std::cout << "SAVING/AFTER <"  << static_cast<int>(m_split_buffer.get()[0])
          << "><" << static_cast<int>(m_split_buffer.get()[1])
          << "><" << static_cast<int>(m_split_buffer.get()[2])
          << "><" << static_cast<int>(m_split_buffer.get()[3]) << ">" << std::endl;
         break;
      }

      // Get size of next item on the stream
      input_stream.ReadLittleEndian32(&msg_len);
std::cout << "[POP_RECORDS] Popping next message, msg_len = " << msg_len << std::endl;
   } while(popRecord(msg_len, input_stream));
}



template<typename DataType>
bool IStreamBuffer<DataType>::popRecord(int msg_len, google::protobuf::io::CodedInputStream &input_stream)
{
   const char *buf_ptr;
   int buf_len;

   if(msg_len > m_max_msglen) {
      throw XrdSsiException("Data record size exceeds XRootD SSI buffer size");
   }

   if(!input_stream.GetDirectBufferPointer(reinterpret_cast<const void**>(&buf_ptr), &buf_len)) buf_len = 0;
std::cout << "[POP_RECORD] buf_len = " << buf_len << ", msg_len = " << msg_len << std::endl;

   if(buf_len < msg_len) {
std::cout << "[POP_RECORD] Saving split of 4+" << buf_len << " bytes" << std::endl;
      // Record payload is split across the boundary, save the partial record

      google::protobuf::io::CodedOutputStream::WriteLittleEndian32ToArray(msg_len, m_split_buffer.get());
      memcpy(m_split_buffer.get() + sizeof(uint32_t), buf_ptr, buf_len);
      m_split_buflen = buf_len + sizeof(uint32_t);

      return false;
   } else {
      DataType record;

      record.ParseFromArray(buf_ptr, msg_len);
      input_stream.Skip(msg_len);

      // If the message terminates at the end of the buffer, we are done, otherwise keep going
      return buf_len != msg_len;
   }
}



#if 0
/*!
 * Data callback.
 *
 * Defines how Data/Stream messages should be handled
 */
template<>
void XrdSsiPbRequestType::DataCallback(XrdSsiRequest::PRD_Xeq &post_process, char *response_bufptr, int response_buflen)
{
   IStreamBuffer istream;

   istream.push(response_bufptr, response_buflen);
   google::protobuf::io::CodedInputStream coded_stream(reinterpret_cast<const uint8_t*>(response_bufptr), response_buflen);


      //OutputJsonString(std::cout, &line_item);

      std::cout << std::setfill(' ') << std::setw(7)  << std::right << line_item.af().archive_file_id() << ' '
                << std::setfill(' ') << std::setw(7)  << std::right << line_item.copy_nb()              << ' '
                << std::setfill(' ') << std::setw(7)  << std::right << line_item.tf().vid()             << ' '
                << std::setfill(' ') << std::setw(7)  << std::right << line_item.tf().f_seq()           << ' '
                << std::setfill(' ') << std::setw(8)  << std::right << line_item.tf().block_id()        << ' '
                << std::setfill(' ') << std::setw(8)  << std::right << line_item.af().disk_instance()   << ' '
                << std::setfill(' ') << std::setw(7)  << std::right << line_item.af().disk_file_id()    << ' '
                << std::setfill(' ') << std::setw(12) << std::right << line_item.af().file_size()       << ' '
                << std::setfill(' ') << std::setw(13) << std::right << line_item.af().cs().type()       << ' '
                << std::setfill(' ') << std::setw(14) << std::right << line_item.af().cs().value()      << ' '
                << std::setfill(' ') << std::setw(13) << std::right << line_item.af().storage_class()   << ' '
                << std::setfill(' ') << std::setw(8)  << std::right << line_item.af().df().owner()      << ' '
                << std::setfill(' ') << std::setw(8)  << std::right << line_item.af().df().group()      << ' '
                << std::setfill(' ') << std::setw(13) << std::right << line_item.af().creation_time()   << ' '
                << line_item.af().df().path() << std::endl;
}
#endif

} // namespace XrdSsiPb

