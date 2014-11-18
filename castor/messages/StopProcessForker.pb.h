// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: StopProcessForker.proto

#ifndef PROTOBUF_StopProcessForker_2eproto__INCLUDED
#define PROTOBUF_StopProcessForker_2eproto__INCLUDED

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
void  protobuf_AddDesc_StopProcessForker_2eproto();
void protobuf_AssignDesc_StopProcessForker_2eproto();
void protobuf_ShutdownFile_StopProcessForker_2eproto();

class StopProcessForker;

// ===================================================================

class StopProcessForker : public ::google::protobuf::Message {
 public:
  StopProcessForker();
  virtual ~StopProcessForker();
  
  StopProcessForker(const StopProcessForker& from);
  
  inline StopProcessForker& operator=(const StopProcessForker& from) {
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
  static const StopProcessForker& default_instance();
  
  void Swap(StopProcessForker* other);
  
  // implements Message ----------------------------------------------
  
  StopProcessForker* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const StopProcessForker& from);
  void MergeFrom(const StopProcessForker& from);
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
  
  // required string reason = 1;
  inline bool has_reason() const;
  inline void clear_reason();
  static const int kReasonFieldNumber = 1;
  inline const ::std::string& reason() const;
  inline void set_reason(const ::std::string& value);
  inline void set_reason(const char* value);
  inline void set_reason(const char* value, size_t size);
  inline ::std::string* mutable_reason();
  
  // @@protoc_insertion_point(class_scope:castor.messages.StopProcessForker)
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  ::std::string* reason_;
  static const ::std::string _default_reason_;
  friend void  protobuf_AddDesc_StopProcessForker_2eproto();
  friend void protobuf_AssignDesc_StopProcessForker_2eproto();
  friend void protobuf_ShutdownFile_StopProcessForker_2eproto();
  
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
  static StopProcessForker* default_instance_;
};
// ===================================================================


// ===================================================================

// StopProcessForker

// required string reason = 1;
inline bool StopProcessForker::has_reason() const {
  return _has_bit(0);
}
inline void StopProcessForker::clear_reason() {
  if (reason_ != &_default_reason_) {
    reason_->clear();
  }
  _clear_bit(0);
}
inline const ::std::string& StopProcessForker::reason() const {
  return *reason_;
}
inline void StopProcessForker::set_reason(const ::std::string& value) {
  _set_bit(0);
  if (reason_ == &_default_reason_) {
    reason_ = new ::std::string;
  }
  reason_->assign(value);
}
inline void StopProcessForker::set_reason(const char* value) {
  _set_bit(0);
  if (reason_ == &_default_reason_) {
    reason_ = new ::std::string;
  }
  reason_->assign(value);
}
inline void StopProcessForker::set_reason(const char* value, size_t size) {
  _set_bit(0);
  if (reason_ == &_default_reason_) {
    reason_ = new ::std::string;
  }
  reason_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* StopProcessForker::mutable_reason() {
  _set_bit(0);
  if (reason_ == &_default_reason_) {
    reason_ = new ::std::string;
  }
  return reason_;
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

#endif  // PROTOBUF_StopProcessForker_2eproto__INCLUDED
