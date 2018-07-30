// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: henangameproto.proto

#ifndef PROTOBUF_henangameproto_2eproto__INCLUDED
#define PROTOBUF_henangameproto_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2005000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2005000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace proto {
namespace game {
namespace henanmj {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_henangameproto_2eproto();
void protobuf_AssignDesc_henangameproto_2eproto();
void protobuf_ShutdownFile_henangameproto_2eproto();

class tagGameEnd;
class AddFanInfo;
class FanInfo;

// ===================================================================

class tagGameEnd : public ::google::protobuf::Message {
 public:
  tagGameEnd();
  virtual ~tagGameEnd();

  tagGameEnd(const tagGameEnd& from);

  inline tagGameEnd& operator=(const tagGameEnd& from) {
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
  static const tagGameEnd& default_instance();

  void Swap(tagGameEnd* other);

  // implements Message ----------------------------------------------

  tagGameEnd* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const tagGameEnd& from);
  void MergeFrom(const tagGameEnd& from);
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

  // repeated .proto.game.henanmj.FanInfo faninfo = 1;
  inline int faninfo_size() const;
  inline void clear_faninfo();
  static const int kFaninfoFieldNumber = 1;
  inline const ::proto::game::henanmj::FanInfo& faninfo(int index) const;
  inline ::proto::game::henanmj::FanInfo* mutable_faninfo(int index);
  inline ::proto::game::henanmj::FanInfo* add_faninfo();
  inline const ::google::protobuf::RepeatedPtrField< ::proto::game::henanmj::FanInfo >&
      faninfo() const;
  inline ::google::protobuf::RepeatedPtrField< ::proto::game::henanmj::FanInfo >*
      mutable_faninfo();

  // repeated int32 piaoNum = 2;
  inline int piaonum_size() const;
  inline void clear_piaonum();
  static const int kPiaoNumFieldNumber = 2;
  inline ::google::protobuf::int32 piaonum(int index) const;
  inline void set_piaonum(int index, ::google::protobuf::int32 value);
  inline void add_piaonum(::google::protobuf::int32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
      piaonum() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
      mutable_piaonum();

  // @@protoc_insertion_point(class_scope:proto.game.henanmj.tagGameEnd)
 private:

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::RepeatedPtrField< ::proto::game::henanmj::FanInfo > faninfo_;
  ::google::protobuf::RepeatedField< ::google::protobuf::int32 > piaonum_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];

  friend void  protobuf_AddDesc_henangameproto_2eproto();
  friend void protobuf_AssignDesc_henangameproto_2eproto();
  friend void protobuf_ShutdownFile_henangameproto_2eproto();

  void InitAsDefaultInstance();
  static tagGameEnd* default_instance_;
};
// -------------------------------------------------------------------

class AddFanInfo : public ::google::protobuf::Message {
 public:
  AddFanInfo();
  virtual ~AddFanInfo();

  AddFanInfo(const AddFanInfo& from);

  inline AddFanInfo& operator=(const AddFanInfo& from) {
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
  static const AddFanInfo& default_instance();

  void Swap(AddFanInfo* other);

  // implements Message ----------------------------------------------

  AddFanInfo* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const AddFanInfo& from);
  void MergeFrom(const AddFanInfo& from);
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

  // optional int32 AddType = 1;
  inline bool has_addtype() const;
  inline void clear_addtype();
  static const int kAddTypeFieldNumber = 1;
  inline ::google::protobuf::int32 addtype() const;
  inline void set_addtype(::google::protobuf::int32 value);

  // optional int32 AddNum = 2;
  inline bool has_addnum() const;
  inline void clear_addnum();
  static const int kAddNumFieldNumber = 2;
  inline ::google::protobuf::int32 addnum() const;
  inline void set_addnum(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:proto.game.henanmj.AddFanInfo)
 private:
  inline void set_has_addtype();
  inline void clear_has_addtype();
  inline void set_has_addnum();
  inline void clear_has_addnum();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::int32 addtype_;
  ::google::protobuf::int32 addnum_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];

  friend void  protobuf_AddDesc_henangameproto_2eproto();
  friend void protobuf_AssignDesc_henangameproto_2eproto();
  friend void protobuf_ShutdownFile_henangameproto_2eproto();

  void InitAsDefaultInstance();
  static AddFanInfo* default_instance_;
};
// -------------------------------------------------------------------

class FanInfo : public ::google::protobuf::Message {
 public:
  FanInfo();
  virtual ~FanInfo();

  FanInfo(const FanInfo& from);

  inline FanInfo& operator=(const FanInfo& from) {
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
  static const FanInfo& default_instance();

  void Swap(FanInfo* other);

  // implements Message ----------------------------------------------

  FanInfo* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const FanInfo& from);
  void MergeFrom(const FanInfo& from);
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

  // required int32 chairID = 1;
  inline bool has_chairid() const;
  inline void clear_chairid();
  static const int kChairIDFieldNumber = 1;
  inline ::google::protobuf::int32 chairid() const;
  inline void set_chairid(::google::protobuf::int32 value);

  // required int32 SpecialType = 2;
  inline bool has_specialtype() const;
  inline void clear_specialtype();
  static const int kSpecialTypeFieldNumber = 2;
  inline ::google::protobuf::int32 specialtype() const;
  inline void set_specialtype(::google::protobuf::int32 value);

  // required int32 FanNum = 3;
  inline bool has_fannum() const;
  inline void clear_fannum();
  static const int kFanNumFieldNumber = 3;
  inline ::google::protobuf::int32 fannum() const;
  inline void set_fannum(::google::protobuf::int32 value);

  // repeated .proto.game.henanmj.AddFanInfo addfan = 4;
  inline int addfan_size() const;
  inline void clear_addfan();
  static const int kAddfanFieldNumber = 4;
  inline const ::proto::game::henanmj::AddFanInfo& addfan(int index) const;
  inline ::proto::game::henanmj::AddFanInfo* mutable_addfan(int index);
  inline ::proto::game::henanmj::AddFanInfo* add_addfan();
  inline const ::google::protobuf::RepeatedPtrField< ::proto::game::henanmj::AddFanInfo >&
      addfan() const;
  inline ::google::protobuf::RepeatedPtrField< ::proto::game::henanmj::AddFanInfo >*
      mutable_addfan();

  // @@protoc_insertion_point(class_scope:proto.game.henanmj.FanInfo)
 private:
  inline void set_has_chairid();
  inline void clear_has_chairid();
  inline void set_has_specialtype();
  inline void clear_has_specialtype();
  inline void set_has_fannum();
  inline void clear_has_fannum();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::int32 chairid_;
  ::google::protobuf::int32 specialtype_;
  ::google::protobuf::RepeatedPtrField< ::proto::game::henanmj::AddFanInfo > addfan_;
  ::google::protobuf::int32 fannum_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(4 + 31) / 32];

  friend void  protobuf_AddDesc_henangameproto_2eproto();
  friend void protobuf_AssignDesc_henangameproto_2eproto();
  friend void protobuf_ShutdownFile_henangameproto_2eproto();

  void InitAsDefaultInstance();
  static FanInfo* default_instance_;
};
// ===================================================================


// ===================================================================

// tagGameEnd

// repeated .proto.game.henanmj.FanInfo faninfo = 1;
inline int tagGameEnd::faninfo_size() const {
  return faninfo_.size();
}
inline void tagGameEnd::clear_faninfo() {
  faninfo_.Clear();
}
inline const ::proto::game::henanmj::FanInfo& tagGameEnd::faninfo(int index) const {
  return faninfo_.Get(index);
}
inline ::proto::game::henanmj::FanInfo* tagGameEnd::mutable_faninfo(int index) {
  return faninfo_.Mutable(index);
}
inline ::proto::game::henanmj::FanInfo* tagGameEnd::add_faninfo() {
  return faninfo_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::proto::game::henanmj::FanInfo >&
tagGameEnd::faninfo() const {
  return faninfo_;
}
inline ::google::protobuf::RepeatedPtrField< ::proto::game::henanmj::FanInfo >*
tagGameEnd::mutable_faninfo() {
  return &faninfo_;
}

// repeated int32 piaoNum = 2;
inline int tagGameEnd::piaonum_size() const {
  return piaonum_.size();
}
inline void tagGameEnd::clear_piaonum() {
  piaonum_.Clear();
}
inline ::google::protobuf::int32 tagGameEnd::piaonum(int index) const {
  return piaonum_.Get(index);
}
inline void tagGameEnd::set_piaonum(int index, ::google::protobuf::int32 value) {
  piaonum_.Set(index, value);
}
inline void tagGameEnd::add_piaonum(::google::protobuf::int32 value) {
  piaonum_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
tagGameEnd::piaonum() const {
  return piaonum_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
tagGameEnd::mutable_piaonum() {
  return &piaonum_;
}

// -------------------------------------------------------------------

// AddFanInfo

// optional int32 AddType = 1;
inline bool AddFanInfo::has_addtype() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void AddFanInfo::set_has_addtype() {
  _has_bits_[0] |= 0x00000001u;
}
inline void AddFanInfo::clear_has_addtype() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void AddFanInfo::clear_addtype() {
  addtype_ = 0;
  clear_has_addtype();
}
inline ::google::protobuf::int32 AddFanInfo::addtype() const {
  return addtype_;
}
inline void AddFanInfo::set_addtype(::google::protobuf::int32 value) {
  set_has_addtype();
  addtype_ = value;
}

// optional int32 AddNum = 2;
inline bool AddFanInfo::has_addnum() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void AddFanInfo::set_has_addnum() {
  _has_bits_[0] |= 0x00000002u;
}
inline void AddFanInfo::clear_has_addnum() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void AddFanInfo::clear_addnum() {
  addnum_ = 0;
  clear_has_addnum();
}
inline ::google::protobuf::int32 AddFanInfo::addnum() const {
  return addnum_;
}
inline void AddFanInfo::set_addnum(::google::protobuf::int32 value) {
  set_has_addnum();
  addnum_ = value;
}

// -------------------------------------------------------------------

// FanInfo

// required int32 chairID = 1;
inline bool FanInfo::has_chairid() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void FanInfo::set_has_chairid() {
  _has_bits_[0] |= 0x00000001u;
}
inline void FanInfo::clear_has_chairid() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void FanInfo::clear_chairid() {
  chairid_ = 0;
  clear_has_chairid();
}
inline ::google::protobuf::int32 FanInfo::chairid() const {
  return chairid_;
}
inline void FanInfo::set_chairid(::google::protobuf::int32 value) {
  set_has_chairid();
  chairid_ = value;
}

// required int32 SpecialType = 2;
inline bool FanInfo::has_specialtype() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void FanInfo::set_has_specialtype() {
  _has_bits_[0] |= 0x00000002u;
}
inline void FanInfo::clear_has_specialtype() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void FanInfo::clear_specialtype() {
  specialtype_ = 0;
  clear_has_specialtype();
}
inline ::google::protobuf::int32 FanInfo::specialtype() const {
  return specialtype_;
}
inline void FanInfo::set_specialtype(::google::protobuf::int32 value) {
  set_has_specialtype();
  specialtype_ = value;
}

// required int32 FanNum = 3;
inline bool FanInfo::has_fannum() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void FanInfo::set_has_fannum() {
  _has_bits_[0] |= 0x00000004u;
}
inline void FanInfo::clear_has_fannum() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void FanInfo::clear_fannum() {
  fannum_ = 0;
  clear_has_fannum();
}
inline ::google::protobuf::int32 FanInfo::fannum() const {
  return fannum_;
}
inline void FanInfo::set_fannum(::google::protobuf::int32 value) {
  set_has_fannum();
  fannum_ = value;
}

// repeated .proto.game.henanmj.AddFanInfo addfan = 4;
inline int FanInfo::addfan_size() const {
  return addfan_.size();
}
inline void FanInfo::clear_addfan() {
  addfan_.Clear();
}
inline const ::proto::game::henanmj::AddFanInfo& FanInfo::addfan(int index) const {
  return addfan_.Get(index);
}
inline ::proto::game::henanmj::AddFanInfo* FanInfo::mutable_addfan(int index) {
  return addfan_.Mutable(index);
}
inline ::proto::game::henanmj::AddFanInfo* FanInfo::add_addfan() {
  return addfan_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::proto::game::henanmj::AddFanInfo >&
FanInfo::addfan() const {
  return addfan_;
}
inline ::google::protobuf::RepeatedPtrField< ::proto::game::henanmj::AddFanInfo >*
FanInfo::mutable_addfan() {
  return &addfan_;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace henanmj
}  // namespace game
}  // namespace proto

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_henangameproto_2eproto__INCLUDED
