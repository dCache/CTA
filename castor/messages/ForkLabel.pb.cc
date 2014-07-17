// Generated by the protocol buffer compiler.  DO NOT EDIT!

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "ForkLabel.pb.h"
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

const ::google::protobuf::Descriptor* ForkLabel_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  ForkLabel_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_ForkLabel_2eproto() {
  protobuf_AddDesc_ForkLabel_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "ForkLabel.proto");
  GOOGLE_CHECK(file != NULL);
  ForkLabel_descriptor_ = file->message_type(0);
  static const int ForkLabel_offsets_[10] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ForkLabel, unitname_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ForkLabel, dgn_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ForkLabel, devfilename_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ForkLabel, density_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ForkLabel, libraryslot_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ForkLabel, devtype_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ForkLabel, force_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ForkLabel, uid_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ForkLabel, gid_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ForkLabel, vid_),
  };
  ForkLabel_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      ForkLabel_descriptor_,
      ForkLabel::default_instance_,
      ForkLabel_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ForkLabel, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ForkLabel, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(ForkLabel));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_ForkLabel_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    ForkLabel_descriptor_, &ForkLabel::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_ForkLabel_2eproto() {
  delete ForkLabel::default_instance_;
  delete ForkLabel_reflection_;
}

void protobuf_AddDesc_ForkLabel_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\017ForkLabel.proto\022\017castor.messages\"\254\001\n\tF"
    "orkLabel\022\020\n\010unitname\030\001 \002(\t\022\013\n\003dgn\030\002 \002(\t\022"
    "\023\n\013devfilename\030\003 \002(\t\022\017\n\007density\030\004 \003(\t\022\023\n"
    "\013libraryslot\030\005 \002(\t\022\017\n\007devtype\030\006 \002(\t\022\r\n\005f"
    "orce\030\007 \002(\010\022\013\n\003uid\030\010 \002(\r\022\013\n\003gid\030\t \002(\r\022\013\n\003"
    "vid\030\n \002(\t", 209);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "ForkLabel.proto", &protobuf_RegisterTypes);
  ForkLabel::default_instance_ = new ForkLabel();
  ForkLabel::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_ForkLabel_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_ForkLabel_2eproto {
  StaticDescriptorInitializer_ForkLabel_2eproto() {
    protobuf_AddDesc_ForkLabel_2eproto();
  }
} static_descriptor_initializer_ForkLabel_2eproto_;


// ===================================================================

const ::std::string ForkLabel::_default_unitname_;
const ::std::string ForkLabel::_default_dgn_;
const ::std::string ForkLabel::_default_devfilename_;
const ::std::string ForkLabel::_default_libraryslot_;
const ::std::string ForkLabel::_default_devtype_;
const ::std::string ForkLabel::_default_vid_;
#ifndef _MSC_VER
const int ForkLabel::kUnitnameFieldNumber;
const int ForkLabel::kDgnFieldNumber;
const int ForkLabel::kDevfilenameFieldNumber;
const int ForkLabel::kDensityFieldNumber;
const int ForkLabel::kLibraryslotFieldNumber;
const int ForkLabel::kDevtypeFieldNumber;
const int ForkLabel::kForceFieldNumber;
const int ForkLabel::kUidFieldNumber;
const int ForkLabel::kGidFieldNumber;
const int ForkLabel::kVidFieldNumber;
#endif  // !_MSC_VER

ForkLabel::ForkLabel()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void ForkLabel::InitAsDefaultInstance() {
}

ForkLabel::ForkLabel(const ForkLabel& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void ForkLabel::SharedCtor() {
  _cached_size_ = 0;
  unitname_ = const_cast< ::std::string*>(&_default_unitname_);
  dgn_ = const_cast< ::std::string*>(&_default_dgn_);
  devfilename_ = const_cast< ::std::string*>(&_default_devfilename_);
  libraryslot_ = const_cast< ::std::string*>(&_default_libraryslot_);
  devtype_ = const_cast< ::std::string*>(&_default_devtype_);
  force_ = false;
  uid_ = 0u;
  gid_ = 0u;
  vid_ = const_cast< ::std::string*>(&_default_vid_);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

ForkLabel::~ForkLabel() {
  SharedDtor();
}

void ForkLabel::SharedDtor() {
  if (unitname_ != &_default_unitname_) {
    delete unitname_;
  }
  if (dgn_ != &_default_dgn_) {
    delete dgn_;
  }
  if (devfilename_ != &_default_devfilename_) {
    delete devfilename_;
  }
  if (libraryslot_ != &_default_libraryslot_) {
    delete libraryslot_;
  }
  if (devtype_ != &_default_devtype_) {
    delete devtype_;
  }
  if (vid_ != &_default_vid_) {
    delete vid_;
  }
  if (this != default_instance_) {
  }
}

void ForkLabel::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* ForkLabel::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return ForkLabel_descriptor_;
}

const ForkLabel& ForkLabel::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_ForkLabel_2eproto();  return *default_instance_;
}

ForkLabel* ForkLabel::default_instance_ = NULL;

ForkLabel* ForkLabel::New() const {
  return new ForkLabel;
}

void ForkLabel::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (_has_bit(0)) {
      if (unitname_ != &_default_unitname_) {
        unitname_->clear();
      }
    }
    if (_has_bit(1)) {
      if (dgn_ != &_default_dgn_) {
        dgn_->clear();
      }
    }
    if (_has_bit(2)) {
      if (devfilename_ != &_default_devfilename_) {
        devfilename_->clear();
      }
    }
    if (_has_bit(4)) {
      if (libraryslot_ != &_default_libraryslot_) {
        libraryslot_->clear();
      }
    }
    if (_has_bit(5)) {
      if (devtype_ != &_default_devtype_) {
        devtype_->clear();
      }
    }
    force_ = false;
    uid_ = 0u;
  }
  if (_has_bits_[8 / 32] & (0xffu << (8 % 32))) {
    gid_ = 0u;
    if (_has_bit(9)) {
      if (vid_ != &_default_vid_) {
        vid_->clear();
      }
    }
  }
  density_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool ForkLabel::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required string unitname = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_unitname()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->unitname().data(), this->unitname().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(18)) goto parse_dgn;
        break;
      }
      
      // required string dgn = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_dgn:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_dgn()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->dgn().data(), this->dgn().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(26)) goto parse_devfilename;
        break;
      }
      
      // required string devfilename = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_devfilename:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_devfilename()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->devfilename().data(), this->devfilename().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(34)) goto parse_density;
        break;
      }
      
      // repeated string density = 4;
      case 4: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_density:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->add_density()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->density(0).data(), this->density(0).length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(34)) goto parse_density;
        if (input->ExpectTag(42)) goto parse_libraryslot;
        break;
      }
      
      // required string libraryslot = 5;
      case 5: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_libraryslot:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_libraryslot()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->libraryslot().data(), this->libraryslot().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(50)) goto parse_devtype;
        break;
      }
      
      // required string devtype = 6;
      case 6: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_devtype:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_devtype()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->devtype().data(), this->devtype().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(56)) goto parse_force;
        break;
      }
      
      // required bool force = 7;
      case 7: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_force:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   bool, ::google::protobuf::internal::WireFormatLite::TYPE_BOOL>(
                 input, &force_)));
          _set_bit(6);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(64)) goto parse_uid;
        break;
      }
      
      // required uint32 uid = 8;
      case 8: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_uid:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &uid_)));
          _set_bit(7);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(72)) goto parse_gid;
        break;
      }
      
      // required uint32 gid = 9;
      case 9: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_gid:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &gid_)));
          _set_bit(8);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(82)) goto parse_vid;
        break;
      }
      
      // required string vid = 10;
      case 10: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_vid:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_vid()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->vid().data(), this->vid().length(),
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

void ForkLabel::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required string unitname = 1;
  if (_has_bit(0)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->unitname().data(), this->unitname().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      1, this->unitname(), output);
  }
  
  // required string dgn = 2;
  if (_has_bit(1)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->dgn().data(), this->dgn().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      2, this->dgn(), output);
  }
  
  // required string devfilename = 3;
  if (_has_bit(2)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->devfilename().data(), this->devfilename().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      3, this->devfilename(), output);
  }
  
  // repeated string density = 4;
  for (int i = 0; i < this->density_size(); i++) {
  ::google::protobuf::internal::WireFormat::VerifyUTF8String(
    this->density(i).data(), this->density(i).length(),
    ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      4, this->density(i), output);
  }
  
  // required string libraryslot = 5;
  if (_has_bit(4)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->libraryslot().data(), this->libraryslot().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      5, this->libraryslot(), output);
  }
  
  // required string devtype = 6;
  if (_has_bit(5)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->devtype().data(), this->devtype().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      6, this->devtype(), output);
  }
  
  // required bool force = 7;
  if (_has_bit(6)) {
    ::google::protobuf::internal::WireFormatLite::WriteBool(7, this->force(), output);
  }
  
  // required uint32 uid = 8;
  if (_has_bit(7)) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(8, this->uid(), output);
  }
  
  // required uint32 gid = 9;
  if (_has_bit(8)) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(9, this->gid(), output);
  }
  
  // required string vid = 10;
  if (_has_bit(9)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->vid().data(), this->vid().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      10, this->vid(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* ForkLabel::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required string unitname = 1;
  if (_has_bit(0)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->unitname().data(), this->unitname().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->unitname(), target);
  }
  
  // required string dgn = 2;
  if (_has_bit(1)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->dgn().data(), this->dgn().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        2, this->dgn(), target);
  }
  
  // required string devfilename = 3;
  if (_has_bit(2)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->devfilename().data(), this->devfilename().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        3, this->devfilename(), target);
  }
  
  // repeated string density = 4;
  for (int i = 0; i < this->density_size(); i++) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->density(i).data(), this->density(i).length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target = ::google::protobuf::internal::WireFormatLite::
      WriteStringToArray(4, this->density(i), target);
  }
  
  // required string libraryslot = 5;
  if (_has_bit(4)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->libraryslot().data(), this->libraryslot().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        5, this->libraryslot(), target);
  }
  
  // required string devtype = 6;
  if (_has_bit(5)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->devtype().data(), this->devtype().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        6, this->devtype(), target);
  }
  
  // required bool force = 7;
  if (_has_bit(6)) {
    target = ::google::protobuf::internal::WireFormatLite::WriteBoolToArray(7, this->force(), target);
  }
  
  // required uint32 uid = 8;
  if (_has_bit(7)) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(8, this->uid(), target);
  }
  
  // required uint32 gid = 9;
  if (_has_bit(8)) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(9, this->gid(), target);
  }
  
  // required string vid = 10;
  if (_has_bit(9)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->vid().data(), this->vid().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        10, this->vid(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int ForkLabel::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required string unitname = 1;
    if (has_unitname()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->unitname());
    }
    
    // required string dgn = 2;
    if (has_dgn()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->dgn());
    }
    
    // required string devfilename = 3;
    if (has_devfilename()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->devfilename());
    }
    
    // required string libraryslot = 5;
    if (has_libraryslot()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->libraryslot());
    }
    
    // required string devtype = 6;
    if (has_devtype()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->devtype());
    }
    
    // required bool force = 7;
    if (has_force()) {
      total_size += 1 + 1;
    }
    
    // required uint32 uid = 8;
    if (has_uid()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt32Size(
          this->uid());
    }
    
  }
  if (_has_bits_[8 / 32] & (0xffu << (8 % 32))) {
    // required uint32 gid = 9;
    if (has_gid()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt32Size(
          this->gid());
    }
    
    // required string vid = 10;
    if (has_vid()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->vid());
    }
    
  }
  // repeated string density = 4;
  total_size += 1 * this->density_size();
  for (int i = 0; i < this->density_size(); i++) {
    total_size += ::google::protobuf::internal::WireFormatLite::StringSize(
      this->density(i));
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

void ForkLabel::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const ForkLabel* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const ForkLabel*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void ForkLabel::MergeFrom(const ForkLabel& from) {
  GOOGLE_CHECK_NE(&from, this);
  density_.MergeFrom(from.density_);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from._has_bit(0)) {
      set_unitname(from.unitname());
    }
    if (from._has_bit(1)) {
      set_dgn(from.dgn());
    }
    if (from._has_bit(2)) {
      set_devfilename(from.devfilename());
    }
    if (from._has_bit(4)) {
      set_libraryslot(from.libraryslot());
    }
    if (from._has_bit(5)) {
      set_devtype(from.devtype());
    }
    if (from._has_bit(6)) {
      set_force(from.force());
    }
    if (from._has_bit(7)) {
      set_uid(from.uid());
    }
  }
  if (from._has_bits_[8 / 32] & (0xffu << (8 % 32))) {
    if (from._has_bit(8)) {
      set_gid(from.gid());
    }
    if (from._has_bit(9)) {
      set_vid(from.vid());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void ForkLabel::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void ForkLabel::CopyFrom(const ForkLabel& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool ForkLabel::IsInitialized() const {
  if ((_has_bits_[0] & 0x000003f7) != 0x000003f7) return false;
  
  return true;
}

void ForkLabel::Swap(ForkLabel* other) {
  if (other != this) {
    std::swap(unitname_, other->unitname_);
    std::swap(dgn_, other->dgn_);
    std::swap(devfilename_, other->devfilename_);
    density_.Swap(&other->density_);
    std::swap(libraryslot_, other->libraryslot_);
    std::swap(devtype_, other->devtype_);
    std::swap(force_, other->force_);
    std::swap(uid_, other->uid_);
    std::swap(gid_, other->gid_);
    std::swap(vid_, other->vid_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata ForkLabel::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = ForkLabel_descriptor_;
  metadata.reflection = ForkLabel_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace messages
}  // namespace castor

// @@protoc_insertion_point(global_scope)
