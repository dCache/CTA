// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Header.proto

#ifndef PROTOBUF_Header_2eproto__INCLUDED
#define PROTOBUF_Header_2eproto__INCLUDED

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
void  protobuf_AddDesc_Header_2eproto();
void protobuf_AssignDesc_Header_2eproto();
void protobuf_ShutdownFile_Header_2eproto();

class Header;

// ===================================================================

class Header : public ::google::protobuf::Message {
 public:
  Header();
  virtual ~Header();
  
  Header(const Header& from);
  
  inline Header& operator=(const Header& from) {
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
  static const Header& default_instance();
  
  void Swap(Header* other);
  
  // implements Message ----------------------------------------------
  
  Header* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Header& from);
  void MergeFrom(const Header& from);
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
  
  // required uint32 magic = 1;
  inline bool has_magic() const;
  inline void clear_magic();
  static const int kMagicFieldNumber = 1;
  inline ::google::protobuf::uint32 magic() const;
  inline void set_magic(::google::protobuf::uint32 value);
  
  // required uint32 protocoltype = 2;
  inline bool has_protocoltype() const;
  inline void clear_protocoltype();
  static const int kProtocoltypeFieldNumber = 2;
  inline ::google::protobuf::uint32 protocoltype() const;
  inline void set_protocoltype(::google::protobuf::uint32 value);
  
  // required uint32 protocolversion = 3;
  inline bool has_protocolversion() const;
  inline void clear_protocolversion();
  static const int kProtocolversionFieldNumber = 3;
  inline ::google::protobuf::uint32 protocolversion() const;
  inline void set_protocolversion(::google::protobuf::uint32 value);
  
  // required uint32 msgtype = 4;
  inline bool has_msgtype() const;
  inline void clear_msgtype();
  static const int kMsgtypeFieldNumber = 4;
  inline ::google::protobuf::uint32 msgtype() const;
  inline void set_msgtype(::google::protobuf::uint32 value);
  
  // required string bodyhashtype = 5;
  inline bool has_bodyhashtype() const;
  inline void clear_bodyhashtype();
  static const int kBodyhashtypeFieldNumber = 5;
  inline const ::std::string& bodyhashtype() const;
  inline void set_bodyhashtype(const ::std::string& value);
  inline void set_bodyhashtype(const char* value);
  inline void set_bodyhashtype(const char* value, size_t size);
  inline ::std::string* mutable_bodyhashtype();
  
  // required string bodyhashValue = 6;
  inline bool has_bodyhashvalue() const;
  inline void clear_bodyhashvalue();
  static const int kBodyhashValueFieldNumber = 6;
  inline const ::std::string& bodyhashvalue() const;
  inline void set_bodyhashvalue(const ::std::string& value);
  inline void set_bodyhashvalue(const char* value);
  inline void set_bodyhashvalue(const char* value, size_t size);
  inline ::std::string* mutable_bodyhashvalue();
  
  // required string bodysignaturetype = 7;
  inline bool has_bodysignaturetype() const;
  inline void clear_bodysignaturetype();
  static const int kBodysignaturetypeFieldNumber = 7;
  inline const ::std::string& bodysignaturetype() const;
  inline void set_bodysignaturetype(const ::std::string& value);
  inline void set_bodysignaturetype(const char* value);
  inline void set_bodysignaturetype(const char* value, size_t size);
  inline ::std::string* mutable_bodysignaturetype();
  
  // required string bodysignature = 8;
  inline bool has_bodysignature() const;
  inline void clear_bodysignature();
  static const int kBodysignatureFieldNumber = 8;
  inline const ::std::string& bodysignature() const;
  inline void set_bodysignature(const ::std::string& value);
  inline void set_bodysignature(const char* value);
  inline void set_bodysignature(const char* value, size_t size);
  inline ::std::string* mutable_bodysignature();
  
  // @@protoc_insertion_point(class_scope:castor.messages.Header)
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  ::google::protobuf::uint32 magic_;
  ::google::protobuf::uint32 protocoltype_;
  ::google::protobuf::uint32 protocolversion_;
  ::google::protobuf::uint32 msgtype_;
  ::std::string* bodyhashtype_;
  static const ::std::string _default_bodyhashtype_;
  ::std::string* bodyhashvalue_;
  static const ::std::string _default_bodyhashvalue_;
  ::std::string* bodysignaturetype_;
  static const ::std::string _default_bodysignaturetype_;
  ::std::string* bodysignature_;
  static const ::std::string _default_bodysignature_;
  friend void  protobuf_AddDesc_Header_2eproto();
  friend void protobuf_AssignDesc_Header_2eproto();
  friend void protobuf_ShutdownFile_Header_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(8 + 31) / 32];
  
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
  static Header* default_instance_;
};
// ===================================================================


// ===================================================================

// Header

// required uint32 magic = 1;
inline bool Header::has_magic() const {
  return _has_bit(0);
}
inline void Header::clear_magic() {
  magic_ = 0u;
  _clear_bit(0);
}
inline ::google::protobuf::uint32 Header::magic() const {
  return magic_;
}
inline void Header::set_magic(::google::protobuf::uint32 value) {
  _set_bit(0);
  magic_ = value;
}

// required uint32 protocoltype = 2;
inline bool Header::has_protocoltype() const {
  return _has_bit(1);
}
inline void Header::clear_protocoltype() {
  protocoltype_ = 0u;
  _clear_bit(1);
}
inline ::google::protobuf::uint32 Header::protocoltype() const {
  return protocoltype_;
}
inline void Header::set_protocoltype(::google::protobuf::uint32 value) {
  _set_bit(1);
  protocoltype_ = value;
}

// required uint32 protocolversion = 3;
inline bool Header::has_protocolversion() const {
  return _has_bit(2);
}
inline void Header::clear_protocolversion() {
  protocolversion_ = 0u;
  _clear_bit(2);
}
inline ::google::protobuf::uint32 Header::protocolversion() const {
  return protocolversion_;
}
inline void Header::set_protocolversion(::google::protobuf::uint32 value) {
  _set_bit(2);
  protocolversion_ = value;
}

// required uint32 msgtype = 4;
inline bool Header::has_msgtype() const {
  return _has_bit(3);
}
inline void Header::clear_msgtype() {
  msgtype_ = 0u;
  _clear_bit(3);
}
inline ::google::protobuf::uint32 Header::msgtype() const {
  return msgtype_;
}
inline void Header::set_msgtype(::google::protobuf::uint32 value) {
  _set_bit(3);
  msgtype_ = value;
}

// required string bodyhashtype = 5;
inline bool Header::has_bodyhashtype() const {
  return _has_bit(4);
}
inline void Header::clear_bodyhashtype() {
  if (bodyhashtype_ != &_default_bodyhashtype_) {
    bodyhashtype_->clear();
  }
  _clear_bit(4);
}
inline const ::std::string& Header::bodyhashtype() const {
  return *bodyhashtype_;
}
inline void Header::set_bodyhashtype(const ::std::string& value) {
  _set_bit(4);
  if (bodyhashtype_ == &_default_bodyhashtype_) {
    bodyhashtype_ = new ::std::string;
  }
  bodyhashtype_->assign(value);
}
inline void Header::set_bodyhashtype(const char* value) {
  _set_bit(4);
  if (bodyhashtype_ == &_default_bodyhashtype_) {
    bodyhashtype_ = new ::std::string;
  }
  bodyhashtype_->assign(value);
}
inline void Header::set_bodyhashtype(const char* value, size_t size) {
  _set_bit(4);
  if (bodyhashtype_ == &_default_bodyhashtype_) {
    bodyhashtype_ = new ::std::string;
  }
  bodyhashtype_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Header::mutable_bodyhashtype() {
  _set_bit(4);
  if (bodyhashtype_ == &_default_bodyhashtype_) {
    bodyhashtype_ = new ::std::string;
  }
  return bodyhashtype_;
}

// required string bodyhashValue = 6;
inline bool Header::has_bodyhashvalue() const {
  return _has_bit(5);
}
inline void Header::clear_bodyhashvalue() {
  if (bodyhashvalue_ != &_default_bodyhashvalue_) {
    bodyhashvalue_->clear();
  }
  _clear_bit(5);
}
inline const ::std::string& Header::bodyhashvalue() const {
  return *bodyhashvalue_;
}
inline void Header::set_bodyhashvalue(const ::std::string& value) {
  _set_bit(5);
  if (bodyhashvalue_ == &_default_bodyhashvalue_) {
    bodyhashvalue_ = new ::std::string;
  }
  bodyhashvalue_->assign(value);
}
inline void Header::set_bodyhashvalue(const char* value) {
  _set_bit(5);
  if (bodyhashvalue_ == &_default_bodyhashvalue_) {
    bodyhashvalue_ = new ::std::string;
  }
  bodyhashvalue_->assign(value);
}
inline void Header::set_bodyhashvalue(const char* value, size_t size) {
  _set_bit(5);
  if (bodyhashvalue_ == &_default_bodyhashvalue_) {
    bodyhashvalue_ = new ::std::string;
  }
  bodyhashvalue_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Header::mutable_bodyhashvalue() {
  _set_bit(5);
  if (bodyhashvalue_ == &_default_bodyhashvalue_) {
    bodyhashvalue_ = new ::std::string;
  }
  return bodyhashvalue_;
}

// required string bodysignaturetype = 7;
inline bool Header::has_bodysignaturetype() const {
  return _has_bit(6);
}
inline void Header::clear_bodysignaturetype() {
  if (bodysignaturetype_ != &_default_bodysignaturetype_) {
    bodysignaturetype_->clear();
  }
  _clear_bit(6);
}
inline const ::std::string& Header::bodysignaturetype() const {
  return *bodysignaturetype_;
}
inline void Header::set_bodysignaturetype(const ::std::string& value) {
  _set_bit(6);
  if (bodysignaturetype_ == &_default_bodysignaturetype_) {
    bodysignaturetype_ = new ::std::string;
  }
  bodysignaturetype_->assign(value);
}
inline void Header::set_bodysignaturetype(const char* value) {
  _set_bit(6);
  if (bodysignaturetype_ == &_default_bodysignaturetype_) {
    bodysignaturetype_ = new ::std::string;
  }
  bodysignaturetype_->assign(value);
}
inline void Header::set_bodysignaturetype(const char* value, size_t size) {
  _set_bit(6);
  if (bodysignaturetype_ == &_default_bodysignaturetype_) {
    bodysignaturetype_ = new ::std::string;
  }
  bodysignaturetype_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Header::mutable_bodysignaturetype() {
  _set_bit(6);
  if (bodysignaturetype_ == &_default_bodysignaturetype_) {
    bodysignaturetype_ = new ::std::string;
  }
  return bodysignaturetype_;
}

// required string bodysignature = 8;
inline bool Header::has_bodysignature() const {
  return _has_bit(7);
}
inline void Header::clear_bodysignature() {
  if (bodysignature_ != &_default_bodysignature_) {
    bodysignature_->clear();
  }
  _clear_bit(7);
}
inline const ::std::string& Header::bodysignature() const {
  return *bodysignature_;
}
inline void Header::set_bodysignature(const ::std::string& value) {
  _set_bit(7);
  if (bodysignature_ == &_default_bodysignature_) {
    bodysignature_ = new ::std::string;
  }
  bodysignature_->assign(value);
}
inline void Header::set_bodysignature(const char* value) {
  _set_bit(7);
  if (bodysignature_ == &_default_bodysignature_) {
    bodysignature_ = new ::std::string;
  }
  bodysignature_->assign(value);
}
inline void Header::set_bodysignature(const char* value, size_t size) {
  _set_bit(7);
  if (bodysignature_ == &_default_bodysignature_) {
    bodysignature_ = new ::std::string;
  }
  bodysignature_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Header::mutable_bodysignature() {
  _set_bit(7);
  if (bodysignature_ == &_default_bodysignature_) {
    bodysignature_ = new ::std::string;
  }
  return bodysignature_;
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

#endif  // PROTOBUF_Header_2eproto__INCLUDED
