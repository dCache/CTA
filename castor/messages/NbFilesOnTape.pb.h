// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: NbFilesOnTape.proto

#ifndef PROTOBUF_NbFilesOnTape_2eproto__INCLUDED
#define PROTOBUF_NbFilesOnTape_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2003000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2003000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_message_reflection.h>
// @@protoc_insertion_point(includes)

namespace castor {
namespace messages {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_NbFilesOnTape_2eproto();
void protobuf_AssignDesc_NbFilesOnTape_2eproto();
void protobuf_ShutdownFile_NbFilesOnTape_2eproto();

class NbFilesOnTape;

// ===================================================================

class NbFilesOnTape : public ::google::protobuf::Message {
 public:
  NbFilesOnTape();
  virtual ~NbFilesOnTape();
  
  NbFilesOnTape(const NbFilesOnTape& from);
  
  inline NbFilesOnTape& operator=(const NbFilesOnTape& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const NbFilesOnTape& default_instance();
  
  void Swap(NbFilesOnTape* other);
  
  // implements Message ----------------------------------------------
  
  NbFilesOnTape* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const NbFilesOnTape& from);
  void MergeFrom(const NbFilesOnTape& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // required uint32 nbfiles = 1;
  inline bool has_nbfiles() const;
  inline void clear_nbfiles();
  static const int kNbfilesFieldNumber = 1;
  inline ::google::protobuf::uint32 nbfiles() const;
  inline void set_nbfiles(::google::protobuf::uint32 value);
  
  // @@protoc_insertion_point(class_scope:castor.messages.NbFilesOnTape)
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  ::google::protobuf::uint32 nbfiles_;
  friend void  protobuf_AddDesc_NbFilesOnTape_2eproto();
  friend void protobuf_AssignDesc_NbFilesOnTape_2eproto();
  friend void protobuf_ShutdownFile_NbFilesOnTape_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
  // WHY DOES & HAVE LOWER PRECEDENCE THAN != !?
  inline bool _has_bit(int index) const {
    return (_has_bits_[index / 32] & (1u << (index % 32))) != 0;
  }
  inline void _set_bit(int index) {
    _has_bits_[index / 32] |= (1u << (index % 32));
  }
  inline void _clear_bit(int index) {
    _has_bits_[index / 32] &= ~(1u << (index % 32));
  }
  
  void InitAsDefaultInstance();
  static NbFilesOnTape* default_instance_;
};
// ===================================================================


// ===================================================================

// NbFilesOnTape

// required uint32 nbfiles = 1;
inline bool NbFilesOnTape::has_nbfiles() const {
  return _has_bit(0);
}
inline void NbFilesOnTape::clear_nbfiles() {
  nbfiles_ = 0u;
  _clear_bit(0);
}
inline ::google::protobuf::uint32 NbFilesOnTape::nbfiles() const {
  return nbfiles_;
}
inline void NbFilesOnTape::set_nbfiles(::google::protobuf::uint32 value) {
  _set_bit(0);
  nbfiles_ = value;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace messages
}  // namespace castor

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_NbFilesOnTape_2eproto__INCLUDED
