// Generated by the protocol buffer compiler.  DO NOT EDIT!

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "Header.pb.h"
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace castor {
namespace messages {

namespace {

const ::google::protobuf::Descriptor* Header_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  Header_reflection_ = NULL;
const ::google::protobuf::Descriptor* ReturnValue_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  ReturnValue_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_Header_2eproto() {
  protobuf_AddDesc_Header_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "Header.proto");
  GOOGLE_CHECK(file != NULL);
  Header_descriptor_ = file->message_type(0);
  static const int Header_offsets_[8] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Header, magic_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Header, protocoltype_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Header, protocolversion_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Header, reqtype_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Header, bodyhashtype_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Header, bodyhashvalue_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Header, bodysignaturetype_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Header, bodysignature_),
  };
  Header_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      Header_descriptor_,
      Header::default_instance_,
      Header_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Header, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Header, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(Header));
  ReturnValue_descriptor_ = file->message_type(1);
  static const int ReturnValue_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ReturnValue, returnvalue_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ReturnValue, message_),
  };
  ReturnValue_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      ReturnValue_descriptor_,
      ReturnValue::default_instance_,
      ReturnValue_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ReturnValue, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ReturnValue, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(ReturnValue));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_Header_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    Header_descriptor_, &Header::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    ReturnValue_descriptor_, &ReturnValue::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_Header_2eproto() {
  delete Header::default_instance_;
  delete Header_reflection_;
  delete ReturnValue::default_instance_;
  delete ReturnValue_reflection_;
}

void protobuf_AddDesc_Header_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\014Header.proto\022\017castor.messages\"\266\001\n\006Head"
    "er\022\r\n\005magic\030\001 \002(\r\022\024\n\014protocolType\030\002 \002(\r\022"
    "\027\n\017protocolVersion\030\003 \002(\r\022\017\n\007reqType\030\004 \002("
    "\r\022\024\n\014bodyHashType\030\005 \002(\t\022\025\n\rbodyHashValue"
    "\030\006 \002(\t\022\031\n\021bodySignatureType\030\007 \002(\t\022\025\n\rbod"
    "ySignature\030\010 \002(\t\"3\n\013ReturnValue\022\023\n\013retur"
    "nValue\030\001 \002(\r\022\017\n\007message\030\002 \002(\t", 269);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "Header.proto", &protobuf_RegisterTypes);
  Header::default_instance_ = new Header();
  ReturnValue::default_instance_ = new ReturnValue();
  Header::default_instance_->InitAsDefaultInstance();
  ReturnValue::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_Header_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_Header_2eproto {
  StaticDescriptorInitializer_Header_2eproto() {
    protobuf_AddDesc_Header_2eproto();
  }
} static_descriptor_initializer_Header_2eproto_;


// ===================================================================

const ::std::string Header::_default_bodyhashtype_;
const ::std::string Header::_default_bodyhashvalue_;
const ::std::string Header::_default_bodysignaturetype_;
const ::std::string Header::_default_bodysignature_;
#ifndef _MSC_VER
const int Header::kMagicFieldNumber;
const int Header::kProtocolTypeFieldNumber;
const int Header::kProtocolVersionFieldNumber;
const int Header::kReqTypeFieldNumber;
const int Header::kBodyHashTypeFieldNumber;
const int Header::kBodyHashValueFieldNumber;
const int Header::kBodySignatureTypeFieldNumber;
const int Header::kBodySignatureFieldNumber;
#endif  // !_MSC_VER

Header::Header()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void Header::InitAsDefaultInstance() {
}

Header::Header(const Header& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void Header::SharedCtor() {
  _cached_size_ = 0;
  magic_ = 0u;
  protocoltype_ = 0u;
  protocolversion_ = 0u;
  reqtype_ = 0u;
  bodyhashtype_ = const_cast< ::std::string*>(&_default_bodyhashtype_);
  bodyhashvalue_ = const_cast< ::std::string*>(&_default_bodyhashvalue_);
  bodysignaturetype_ = const_cast< ::std::string*>(&_default_bodysignaturetype_);
  bodysignature_ = const_cast< ::std::string*>(&_default_bodysignature_);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

Header::~Header() {
  SharedDtor();
}

void Header::SharedDtor() {
  if (bodyhashtype_ != &_default_bodyhashtype_) {
    delete bodyhashtype_;
  }
  if (bodyhashvalue_ != &_default_bodyhashvalue_) {
    delete bodyhashvalue_;
  }
  if (bodysignaturetype_ != &_default_bodysignaturetype_) {
    delete bodysignaturetype_;
  }
  if (bodysignature_ != &_default_bodysignature_) {
    delete bodysignature_;
  }
  if (this != default_instance_) {
  }
}

void Header::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* Header::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Header_descriptor_;
}

const Header& Header::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_Header_2eproto();  return *default_instance_;
}

Header* Header::default_instance_ = NULL;

Header* Header::New() const {
  return new Header;
}

void Header::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    magic_ = 0u;
    protocoltype_ = 0u;
    protocolversion_ = 0u;
    reqtype_ = 0u;
    if (_has_bit(4)) {
      if (bodyhashtype_ != &_default_bodyhashtype_) {
        bodyhashtype_->clear();
      }
    }
    if (_has_bit(5)) {
      if (bodyhashvalue_ != &_default_bodyhashvalue_) {
        bodyhashvalue_->clear();
      }
    }
    if (_has_bit(6)) {
      if (bodysignaturetype_ != &_default_bodysignaturetype_) {
        bodysignaturetype_->clear();
      }
    }
    if (_has_bit(7)) {
      if (bodysignature_ != &_default_bodysignature_) {
        bodysignature_->clear();
      }
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool Header::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required uint32 magic = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &magic_)));
          _set_bit(0);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(16)) goto parse_protocolType;
        break;
      }
      
      // required uint32 protocolType = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_protocolType:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &protocoltype_)));
          _set_bit(1);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(24)) goto parse_protocolVersion;
        break;
      }
      
      // required uint32 protocolVersion = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_protocolVersion:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &protocolversion_)));
          _set_bit(2);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(32)) goto parse_reqType;
        break;
      }
      
      // required uint32 reqType = 4;
      case 4: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_reqType:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &reqtype_)));
          _set_bit(3);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(42)) goto parse_bodyHashType;
        break;
      }
      
      // required string bodyHashType = 5;
      case 5: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_bodyHashType:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_bodyhashtype()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->bodyhashtype().data(), this->bodyhashtype().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(50)) goto parse_bodyHashValue;
        break;
      }
      
      // required string bodyHashValue = 6;
      case 6: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_bodyHashValue:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_bodyhashvalue()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->bodyhashvalue().data(), this->bodyhashvalue().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(58)) goto parse_bodySignatureType;
        break;
      }
      
      // required string bodySignatureType = 7;
      case 7: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_bodySignatureType:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_bodysignaturetype()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->bodysignaturetype().data(), this->bodysignaturetype().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(66)) goto parse_bodySignature;
        break;
      }
      
      // required string bodySignature = 8;
      case 8: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_bodySignature:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_bodysignature()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->bodysignature().data(), this->bodysignature().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }
      
      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void Header::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required uint32 magic = 1;
  if (_has_bit(0)) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(1, this->magic(), output);
  }
  
  // required uint32 protocolType = 2;
  if (_has_bit(1)) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(2, this->protocoltype(), output);
  }
  
  // required uint32 protocolVersion = 3;
  if (_has_bit(2)) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(3, this->protocolversion(), output);
  }
  
  // required uint32 reqType = 4;
  if (_has_bit(3)) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(4, this->reqtype(), output);
  }
  
  // required string bodyHashType = 5;
  if (_has_bit(4)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->bodyhashtype().data(), this->bodyhashtype().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      5, this->bodyhashtype(), output);
  }
  
  // required string bodyHashValue = 6;
  if (_has_bit(5)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->bodyhashvalue().data(), this->bodyhashvalue().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      6, this->bodyhashvalue(), output);
  }
  
  // required string bodySignatureType = 7;
  if (_has_bit(6)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->bodysignaturetype().data(), this->bodysignaturetype().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      7, this->bodysignaturetype(), output);
  }
  
  // required string bodySignature = 8;
  if (_has_bit(7)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->bodysignature().data(), this->bodysignature().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      8, this->bodysignature(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* Header::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required uint32 magic = 1;
  if (_has_bit(0)) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(1, this->magic(), target);
  }
  
  // required uint32 protocolType = 2;
  if (_has_bit(1)) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(2, this->protocoltype(), target);
  }
  
  // required uint32 protocolVersion = 3;
  if (_has_bit(2)) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(3, this->protocolversion(), target);
  }
  
  // required uint32 reqType = 4;
  if (_has_bit(3)) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(4, this->reqtype(), target);
  }
  
  // required string bodyHashType = 5;
  if (_has_bit(4)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->bodyhashtype().data(), this->bodyhashtype().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        5, this->bodyhashtype(), target);
  }
  
  // required string bodyHashValue = 6;
  if (_has_bit(5)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->bodyhashvalue().data(), this->bodyhashvalue().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        6, this->bodyhashvalue(), target);
  }
  
  // required string bodySignatureType = 7;
  if (_has_bit(6)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->bodysignaturetype().data(), this->bodysignaturetype().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        7, this->bodysignaturetype(), target);
  }
  
  // required string bodySignature = 8;
  if (_has_bit(7)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->bodysignature().data(), this->bodysignature().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        8, this->bodysignature(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int Header::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required uint32 magic = 1;
    if (has_magic()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt32Size(
          this->magic());
    }
    
    // required uint32 protocolType = 2;
    if (has_protocoltype()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt32Size(
          this->protocoltype());
    }
    
    // required uint32 protocolVersion = 3;
    if (has_protocolversion()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt32Size(
          this->protocolversion());
    }
    
    // required uint32 reqType = 4;
    if (has_reqtype()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt32Size(
          this->reqtype());
    }
    
    // required string bodyHashType = 5;
    if (has_bodyhashtype()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->bodyhashtype());
    }
    
    // required string bodyHashValue = 6;
    if (has_bodyhashvalue()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->bodyhashvalue());
    }
    
    // required string bodySignatureType = 7;
    if (has_bodysignaturetype()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->bodysignaturetype());
    }
    
    // required string bodySignature = 8;
    if (has_bodysignature()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->bodysignature());
    }
    
  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void Header::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const Header* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const Header*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void Header::MergeFrom(const Header& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from._has_bit(0)) {
      set_magic(from.magic());
    }
    if (from._has_bit(1)) {
      set_protocoltype(from.protocoltype());
    }
    if (from._has_bit(2)) {
      set_protocolversion(from.protocolversion());
    }
    if (from._has_bit(3)) {
      set_reqtype(from.reqtype());
    }
    if (from._has_bit(4)) {
      set_bodyhashtype(from.bodyhashtype());
    }
    if (from._has_bit(5)) {
      set_bodyhashvalue(from.bodyhashvalue());
    }
    if (from._has_bit(6)) {
      set_bodysignaturetype(from.bodysignaturetype());
    }
    if (from._has_bit(7)) {
      set_bodysignature(from.bodysignature());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void Header::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Header::CopyFrom(const Header& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Header::IsInitialized() const {
  if ((_has_bits_[0] & 0x000000ff) != 0x000000ff) return false;
  
  return true;
}

void Header::Swap(Header* other) {
  if (other != this) {
    std::swap(magic_, other->magic_);
    std::swap(protocoltype_, other->protocoltype_);
    std::swap(protocolversion_, other->protocolversion_);
    std::swap(reqtype_, other->reqtype_);
    std::swap(bodyhashtype_, other->bodyhashtype_);
    std::swap(bodyhashvalue_, other->bodyhashvalue_);
    std::swap(bodysignaturetype_, other->bodysignaturetype_);
    std::swap(bodysignature_, other->bodysignature_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata Header::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = Header_descriptor_;
  metadata.reflection = Header_reflection_;
  return metadata;
}


// ===================================================================

const ::std::string ReturnValue::_default_message_;
#ifndef _MSC_VER
const int ReturnValue::kReturnValueFieldNumber;
const int ReturnValue::kMessageFieldNumber;
#endif  // !_MSC_VER

ReturnValue::ReturnValue()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void ReturnValue::InitAsDefaultInstance() {
}

ReturnValue::ReturnValue(const ReturnValue& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void ReturnValue::SharedCtor() {
  _cached_size_ = 0;
  returnvalue_ = 0u;
  message_ = const_cast< ::std::string*>(&_default_message_);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

ReturnValue::~ReturnValue() {
  SharedDtor();
}

void ReturnValue::SharedDtor() {
  if (message_ != &_default_message_) {
    delete message_;
  }
  if (this != default_instance_) {
  }
}

void ReturnValue::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* ReturnValue::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return ReturnValue_descriptor_;
}

const ReturnValue& ReturnValue::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_Header_2eproto();  return *default_instance_;
}

ReturnValue* ReturnValue::default_instance_ = NULL;

ReturnValue* ReturnValue::New() const {
  return new ReturnValue;
}

void ReturnValue::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    returnvalue_ = 0u;
    if (_has_bit(1)) {
      if (message_ != &_default_message_) {
        message_->clear();
      }
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool ReturnValue::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required uint32 returnValue = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &returnvalue_)));
          _set_bit(0);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(18)) goto parse_message;
        break;
      }
      
      // required string message = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_message:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_message()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->message().data(), this->message().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }
      
      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void ReturnValue::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required uint32 returnValue = 1;
  if (_has_bit(0)) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(1, this->returnvalue(), output);
  }
  
  // required string message = 2;
  if (_has_bit(1)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->message().data(), this->message().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      2, this->message(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* ReturnValue::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required uint32 returnValue = 1;
  if (_has_bit(0)) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(1, this->returnvalue(), target);
  }
  
  // required string message = 2;
  if (_has_bit(1)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->message().data(), this->message().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        2, this->message(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int ReturnValue::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required uint32 returnValue = 1;
    if (has_returnvalue()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt32Size(
          this->returnvalue());
    }
    
    // required string message = 2;
    if (has_message()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->message());
    }
    
  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void ReturnValue::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const ReturnValue* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const ReturnValue*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void ReturnValue::MergeFrom(const ReturnValue& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from._has_bit(0)) {
      set_returnvalue(from.returnvalue());
    }
    if (from._has_bit(1)) {
      set_message(from.message());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void ReturnValue::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void ReturnValue::CopyFrom(const ReturnValue& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool ReturnValue::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000003) != 0x00000003) return false;
  
  return true;
}

void ReturnValue::Swap(ReturnValue* other) {
  if (other != this) {
    std::swap(returnvalue_, other->returnvalue_);
    std::swap(message_, other->message_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata ReturnValue::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = ReturnValue_descriptor_;
  metadata.reflection = ReturnValue_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace messages
}  // namespace castor

// @@protoc_insertion_point(global_scope)
