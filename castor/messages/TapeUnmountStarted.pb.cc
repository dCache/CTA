// Generated by the protocol buffer compiler.  DO NOT EDIT!

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "TapeUnmountStarted.pb.h"
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

const ::google::protobuf::Descriptor* TapeUnmountStarted_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  TapeUnmountStarted_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_TapeUnmountStarted_2eproto() {
  protobuf_AddDesc_TapeUnmountStarted_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "TapeUnmountStarted.proto");
  GOOGLE_CHECK(file != NULL);
  TapeUnmountStarted_descriptor_ = file->message_type(0);
  static const int TapeUnmountStarted_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(TapeUnmountStarted, vid_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(TapeUnmountStarted, unitname_),
  };
  TapeUnmountStarted_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      TapeUnmountStarted_descriptor_,
      TapeUnmountStarted::default_instance_,
      TapeUnmountStarted_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(TapeUnmountStarted, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(TapeUnmountStarted, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(TapeUnmountStarted));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_TapeUnmountStarted_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    TapeUnmountStarted_descriptor_, &TapeUnmountStarted::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_TapeUnmountStarted_2eproto() {
  delete TapeUnmountStarted::default_instance_;
  delete TapeUnmountStarted_reflection_;
}

void protobuf_AddDesc_TapeUnmountStarted_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\030TapeUnmountStarted.proto\022\017castor.messa"
    "ges\"3\n\022TapeUnmountStarted\022\013\n\003vid\030\001 \002(\t\022\020"
    "\n\010unitname\030\002 \002(\t", 96);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "TapeUnmountStarted.proto", &protobuf_RegisterTypes);
  TapeUnmountStarted::default_instance_ = new TapeUnmountStarted();
  TapeUnmountStarted::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_TapeUnmountStarted_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_TapeUnmountStarted_2eproto {
  StaticDescriptorInitializer_TapeUnmountStarted_2eproto() {
    protobuf_AddDesc_TapeUnmountStarted_2eproto();
  }
} static_descriptor_initializer_TapeUnmountStarted_2eproto_;


// ===================================================================

const ::std::string TapeUnmountStarted::_default_vid_;
const ::std::string TapeUnmountStarted::_default_unitname_;
#ifndef _MSC_VER
const int TapeUnmountStarted::kVidFieldNumber;
const int TapeUnmountStarted::kUnitnameFieldNumber;
#endif  // !_MSC_VER

TapeUnmountStarted::TapeUnmountStarted()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void TapeUnmountStarted::InitAsDefaultInstance() {
}

TapeUnmountStarted::TapeUnmountStarted(const TapeUnmountStarted& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void TapeUnmountStarted::SharedCtor() {
  _cached_size_ = 0;
  vid_ = const_cast< ::std::string*>(&_default_vid_);
  unitname_ = const_cast< ::std::string*>(&_default_unitname_);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

TapeUnmountStarted::~TapeUnmountStarted() {
  SharedDtor();
}

void TapeUnmountStarted::SharedDtor() {
  if (vid_ != &_default_vid_) {
    delete vid_;
  }
  if (unitname_ != &_default_unitname_) {
    delete unitname_;
  }
  if (this != default_instance_) {
  }
}

void TapeUnmountStarted::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* TapeUnmountStarted::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return TapeUnmountStarted_descriptor_;
}

const TapeUnmountStarted& TapeUnmountStarted::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_TapeUnmountStarted_2eproto();  return *default_instance_;
}

TapeUnmountStarted* TapeUnmountStarted::default_instance_ = NULL;

TapeUnmountStarted* TapeUnmountStarted::New() const {
  return new TapeUnmountStarted;
}

void TapeUnmountStarted::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (_has_bit(0)) {
      if (vid_ != &_default_vid_) {
        vid_->clear();
      }
    }
    if (_has_bit(1)) {
      if (unitname_ != &_default_unitname_) {
        unitname_->clear();
      }
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool TapeUnmountStarted::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required string vid = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_vid()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->vid().data(), this->vid().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(18)) goto parse_unitname;
        break;
      }
      
      // required string unitname = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_unitname:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_unitname()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->unitname().data(), this->unitname().length(),
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

void TapeUnmountStarted::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required string vid = 1;
  if (_has_bit(0)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->vid().data(), this->vid().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      1, this->vid(), output);
  }
  
  // required string unitname = 2;
  if (_has_bit(1)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->unitname().data(), this->unitname().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      2, this->unitname(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* TapeUnmountStarted::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required string vid = 1;
  if (_has_bit(0)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->vid().data(), this->vid().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->vid(), target);
  }
  
  // required string unitname = 2;
  if (_has_bit(1)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->unitname().data(), this->unitname().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        2, this->unitname(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int TapeUnmountStarted::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required string vid = 1;
    if (has_vid()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->vid());
    }
    
    // required string unitname = 2;
    if (has_unitname()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->unitname());
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

void TapeUnmountStarted::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const TapeUnmountStarted* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const TapeUnmountStarted*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void TapeUnmountStarted::MergeFrom(const TapeUnmountStarted& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from._has_bit(0)) {
      set_vid(from.vid());
    }
    if (from._has_bit(1)) {
      set_unitname(from.unitname());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void TapeUnmountStarted::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void TapeUnmountStarted::CopyFrom(const TapeUnmountStarted& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool TapeUnmountStarted::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000003) != 0x00000003) return false;
  
  return true;
}

void TapeUnmountStarted::Swap(TapeUnmountStarted* other) {
  if (other != this) {
    std::swap(vid_, other->vid_);
    std::swap(unitname_, other->unitname_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata TapeUnmountStarted::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = TapeUnmountStarted_descriptor_;
  metadata.reflection = TapeUnmountStarted_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace messages
}  // namespace castor

// @@protoc_insertion_point(global_scope)
