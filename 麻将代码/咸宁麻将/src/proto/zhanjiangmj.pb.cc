// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: zhanjiangmj.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "zhanjiangmj.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace proto {
namespace game {
namespace zhanjiangmj {

namespace {

const ::google::protobuf::Descriptor* GuiCardInfo_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  GuiCardInfo_reflection_ = NULL;
const ::google::protobuf::Descriptor* tagZhanJiangmjGameEnd_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  tagZhanJiangmjGameEnd_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_zhanjiangmj_2eproto() {
  protobuf_AddDesc_zhanjiangmj_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "zhanjiangmj.proto");
  GOOGLE_CHECK(file != NULL);
  GuiCardInfo_descriptor_ = file->message_type(0);
  static const int GuiCardInfo_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(GuiCardInfo, guipai_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(GuiCardInfo, gamestartgui_),
  };
  GuiCardInfo_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      GuiCardInfo_descriptor_,
      GuiCardInfo::default_instance_,
      GuiCardInfo_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(GuiCardInfo, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(GuiCardInfo, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(GuiCardInfo));
  tagZhanJiangmjGameEnd_descriptor_ = file->message_type(1);
  static const int tagZhanJiangmjGameEnd_offsets_[1] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(tagZhanJiangmjGameEnd, zhanjiangmapai_),
  };
  tagZhanJiangmjGameEnd_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      tagZhanJiangmjGameEnd_descriptor_,
      tagZhanJiangmjGameEnd::default_instance_,
      tagZhanJiangmjGameEnd_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(tagZhanJiangmjGameEnd, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(tagZhanJiangmjGameEnd, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(tagZhanJiangmjGameEnd));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_zhanjiangmj_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    GuiCardInfo_descriptor_, &GuiCardInfo::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    tagZhanJiangmjGameEnd_descriptor_, &tagZhanJiangmjGameEnd::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_zhanjiangmj_2eproto() {
  delete GuiCardInfo::default_instance_;
  delete GuiCardInfo_reflection_;
  delete tagZhanJiangmjGameEnd::default_instance_;
  delete tagZhanJiangmjGameEnd_reflection_;
}

void protobuf_AddDesc_zhanjiangmj_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\021zhanjiangmj.proto\022\026proto.game.zhanjian"
    "gmj\"3\n\013GuiCardInfo\022\016\n\006GuiPai\030\001 \001(\005\022\024\n\014Ga"
    "meStartGui\030\002 \001(\010\"/\n\025tagZhanJiangmjGameEn"
    "d\022\026\n\016ZhanJiangMaPai\030\001 \003(\005", 145);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "zhanjiangmj.proto", &protobuf_RegisterTypes);
  GuiCardInfo::default_instance_ = new GuiCardInfo();
  tagZhanJiangmjGameEnd::default_instance_ = new tagZhanJiangmjGameEnd();
  GuiCardInfo::default_instance_->InitAsDefaultInstance();
  tagZhanJiangmjGameEnd::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_zhanjiangmj_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_zhanjiangmj_2eproto {
  StaticDescriptorInitializer_zhanjiangmj_2eproto() {
    protobuf_AddDesc_zhanjiangmj_2eproto();
  }
} static_descriptor_initializer_zhanjiangmj_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int GuiCardInfo::kGuiPaiFieldNumber;
const int GuiCardInfo::kGameStartGuiFieldNumber;
#endif  // !_MSC_VER

GuiCardInfo::GuiCardInfo()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void GuiCardInfo::InitAsDefaultInstance() {
}

GuiCardInfo::GuiCardInfo(const GuiCardInfo& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void GuiCardInfo::SharedCtor() {
  _cached_size_ = 0;
  guipai_ = 0;
  gamestartgui_ = false;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

GuiCardInfo::~GuiCardInfo() {
  SharedDtor();
}

void GuiCardInfo::SharedDtor() {
  if (this != default_instance_) {
  }
}

void GuiCardInfo::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* GuiCardInfo::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return GuiCardInfo_descriptor_;
}

const GuiCardInfo& GuiCardInfo::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_zhanjiangmj_2eproto();
  return *default_instance_;
}

GuiCardInfo* GuiCardInfo::default_instance_ = NULL;

GuiCardInfo* GuiCardInfo::New() const {
  return new GuiCardInfo;
}

void GuiCardInfo::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    guipai_ = 0;
    gamestartgui_ = false;
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool GuiCardInfo::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional int32 GuiPai = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &guipai_)));
          set_has_guipai();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(16)) goto parse_GameStartGui;
        break;
      }

      // optional bool GameStartGui = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_GameStartGui:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   bool, ::google::protobuf::internal::WireFormatLite::TYPE_BOOL>(
                 input, &gamestartgui_)));
          set_has_gamestartgui();
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

void GuiCardInfo::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // optional int32 GuiPai = 1;
  if (has_guipai()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->guipai(), output);
  }

  // optional bool GameStartGui = 2;
  if (has_gamestartgui()) {
    ::google::protobuf::internal::WireFormatLite::WriteBool(2, this->gamestartgui(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* GuiCardInfo::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // optional int32 GuiPai = 1;
  if (has_guipai()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->guipai(), target);
  }

  // optional bool GameStartGui = 2;
  if (has_gamestartgui()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteBoolToArray(2, this->gamestartgui(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int GuiCardInfo::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional int32 GuiPai = 1;
    if (has_guipai()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->guipai());
    }

    // optional bool GameStartGui = 2;
    if (has_gamestartgui()) {
      total_size += 1 + 1;
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

void GuiCardInfo::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const GuiCardInfo* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const GuiCardInfo*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void GuiCardInfo::MergeFrom(const GuiCardInfo& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_guipai()) {
      set_guipai(from.guipai());
    }
    if (from.has_gamestartgui()) {
      set_gamestartgui(from.gamestartgui());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void GuiCardInfo::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void GuiCardInfo::CopyFrom(const GuiCardInfo& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool GuiCardInfo::IsInitialized() const {

  return true;
}

void GuiCardInfo::Swap(GuiCardInfo* other) {
  if (other != this) {
    std::swap(guipai_, other->guipai_);
    std::swap(gamestartgui_, other->gamestartgui_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata GuiCardInfo::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = GuiCardInfo_descriptor_;
  metadata.reflection = GuiCardInfo_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int tagZhanJiangmjGameEnd::kZhanJiangMaPaiFieldNumber;
#endif  // !_MSC_VER

tagZhanJiangmjGameEnd::tagZhanJiangmjGameEnd()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void tagZhanJiangmjGameEnd::InitAsDefaultInstance() {
}

tagZhanJiangmjGameEnd::tagZhanJiangmjGameEnd(const tagZhanJiangmjGameEnd& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void tagZhanJiangmjGameEnd::SharedCtor() {
  _cached_size_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

tagZhanJiangmjGameEnd::~tagZhanJiangmjGameEnd() {
  SharedDtor();
}

void tagZhanJiangmjGameEnd::SharedDtor() {
  if (this != default_instance_) {
  }
}

void tagZhanJiangmjGameEnd::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* tagZhanJiangmjGameEnd::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return tagZhanJiangmjGameEnd_descriptor_;
}

const tagZhanJiangmjGameEnd& tagZhanJiangmjGameEnd::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_zhanjiangmj_2eproto();
  return *default_instance_;
}

tagZhanJiangmjGameEnd* tagZhanJiangmjGameEnd::default_instance_ = NULL;

tagZhanJiangmjGameEnd* tagZhanJiangmjGameEnd::New() const {
  return new tagZhanJiangmjGameEnd;
}

void tagZhanJiangmjGameEnd::Clear() {
  zhanjiangmapai_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool tagZhanJiangmjGameEnd::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // repeated int32 ZhanJiangMaPai = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_ZhanJiangMaPai:
          DO_((::google::protobuf::internal::WireFormatLite::ReadRepeatedPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 1, 8, input, this->mutable_zhanjiangmapai())));
        } else if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag)
                   == ::google::protobuf::internal::WireFormatLite::
                      WIRETYPE_LENGTH_DELIMITED) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPackedPrimitiveNoInline<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, this->mutable_zhanjiangmapai())));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(8)) goto parse_ZhanJiangMaPai;
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

void tagZhanJiangmjGameEnd::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // repeated int32 ZhanJiangMaPai = 1;
  for (int i = 0; i < this->zhanjiangmapai_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(
      1, this->zhanjiangmapai(i), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* tagZhanJiangmjGameEnd::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // repeated int32 ZhanJiangMaPai = 1;
  for (int i = 0; i < this->zhanjiangmapai_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteInt32ToArray(1, this->zhanjiangmapai(i), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int tagZhanJiangmjGameEnd::ByteSize() const {
  int total_size = 0;

  // repeated int32 ZhanJiangMaPai = 1;
  {
    int data_size = 0;
    for (int i = 0; i < this->zhanjiangmapai_size(); i++) {
      data_size += ::google::protobuf::internal::WireFormatLite::
        Int32Size(this->zhanjiangmapai(i));
    }
    total_size += 1 * this->zhanjiangmapai_size() + data_size;
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

void tagZhanJiangmjGameEnd::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const tagZhanJiangmjGameEnd* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const tagZhanJiangmjGameEnd*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void tagZhanJiangmjGameEnd::MergeFrom(const tagZhanJiangmjGameEnd& from) {
  GOOGLE_CHECK_NE(&from, this);
  zhanjiangmapai_.MergeFrom(from.zhanjiangmapai_);
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void tagZhanJiangmjGameEnd::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void tagZhanJiangmjGameEnd::CopyFrom(const tagZhanJiangmjGameEnd& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool tagZhanJiangmjGameEnd::IsInitialized() const {

  return true;
}

void tagZhanJiangmjGameEnd::Swap(tagZhanJiangmjGameEnd* other) {
  if (other != this) {
    zhanjiangmapai_.Swap(&other->zhanjiangmapai_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata tagZhanJiangmjGameEnd::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = tagZhanJiangmjGameEnd_descriptor_;
  metadata.reflection = tagZhanJiangmjGameEnd_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace zhanjiangmj
}  // namespace game
}  // namespace proto

// @@protoc_insertion_point(global_scope)
