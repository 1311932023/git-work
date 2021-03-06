// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: nanpingmahjonggame.proto

#ifndef PROTOBUF_nanpingmahjonggame_2eproto__INCLUDED
#define PROTOBUF_nanpingmahjonggame_2eproto__INCLUDED

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
namespace nanpingmahjonggame {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_nanpingmahjonggame_2eproto();
void protobuf_AssignDesc_nanpingmahjonggame_2eproto();
void protobuf_ShutdownFile_nanpingmahjonggame_2eproto();

class Info;
class JinZhongGangInfo;
class GameEnd;
class ZhongSum;
class NanPingGameSence;

// ===================================================================

class Info : public ::google::protobuf::Message {
 public:
  Info();
  virtual ~Info();

  Info(const Info& from);

  inline Info& operator=(const Info& from) {
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
  static const Info& default_instance();

  void Swap(Info* other);

  // implements Message ----------------------------------------------

  Info* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Info& from);
  void MergeFrom(const Info& from);
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

  // repeated int32 card = 2;
  inline int card_size() const;
  inline void clear_card();
  static const int kCardFieldNumber = 2;
  inline ::google::protobuf::int32 card(int index) const;
  inline void set_card(int index, ::google::protobuf::int32 value);
  inline void add_card(::google::protobuf::int32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
      card() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
      mutable_card();

  // repeated int32 card_num = 3;
  inline int card_num_size() const;
  inline void clear_card_num();
  static const int kCardNumFieldNumber = 3;
  inline ::google::protobuf::int32 card_num(int index) const;
  inline void set_card_num(int index, ::google::protobuf::int32 value);
  inline void add_card_num(::google::protobuf::int32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
      card_num() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
      mutable_card_num();

  // @@protoc_insertion_point(class_scope:proto.game.nanpingmahjonggame.Info)
 private:

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::RepeatedField< ::google::protobuf::int32 > card_;
  ::google::protobuf::RepeatedField< ::google::protobuf::int32 > card_num_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];

  friend void  protobuf_AddDesc_nanpingmahjonggame_2eproto();
  friend void protobuf_AssignDesc_nanpingmahjonggame_2eproto();
  friend void protobuf_ShutdownFile_nanpingmahjonggame_2eproto();

  void InitAsDefaultInstance();
  static Info* default_instance_;
};
// -------------------------------------------------------------------

class JinZhongGangInfo : public ::google::protobuf::Message {
 public:
  JinZhongGangInfo();
  virtual ~JinZhongGangInfo();

  JinZhongGangInfo(const JinZhongGangInfo& from);

  inline JinZhongGangInfo& operator=(const JinZhongGangInfo& from) {
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
  static const JinZhongGangInfo& default_instance();

  void Swap(JinZhongGangInfo* other);

  // implements Message ----------------------------------------------

  JinZhongGangInfo* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const JinZhongGangInfo& from);
  void MergeFrom(const JinZhongGangInfo& from);
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

  // required int32 sum = 1;
  inline bool has_sum() const;
  inline void clear_sum();
  static const int kSumFieldNumber = 1;
  inline ::google::protobuf::int32 sum() const;
  inline void set_sum(::google::protobuf::int32 value);

  // repeated .proto.game.nanpingmahjonggame.Info info = 2;
  inline int info_size() const;
  inline void clear_info();
  static const int kInfoFieldNumber = 2;
  inline const ::proto::game::nanpingmahjonggame::Info& info(int index) const;
  inline ::proto::game::nanpingmahjonggame::Info* mutable_info(int index);
  inline ::proto::game::nanpingmahjonggame::Info* add_info();
  inline const ::google::protobuf::RepeatedPtrField< ::proto::game::nanpingmahjonggame::Info >&
      info() const;
  inline ::google::protobuf::RepeatedPtrField< ::proto::game::nanpingmahjonggame::Info >*
      mutable_info();

  // @@protoc_insertion_point(class_scope:proto.game.nanpingmahjonggame.JinZhongGangInfo)
 private:
  inline void set_has_sum();
  inline void clear_has_sum();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::RepeatedPtrField< ::proto::game::nanpingmahjonggame::Info > info_;
  ::google::protobuf::int32 sum_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];

  friend void  protobuf_AddDesc_nanpingmahjonggame_2eproto();
  friend void protobuf_AssignDesc_nanpingmahjonggame_2eproto();
  friend void protobuf_ShutdownFile_nanpingmahjonggame_2eproto();

  void InitAsDefaultInstance();
  static JinZhongGangInfo* default_instance_;
};
// -------------------------------------------------------------------

class GameEnd : public ::google::protobuf::Message {
 public:
  GameEnd();
  virtual ~GameEnd();

  GameEnd(const GameEnd& from);

  inline GameEnd& operator=(const GameEnd& from) {
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
  static const GameEnd& default_instance();

  void Swap(GameEnd* other);

  // implements Message ----------------------------------------------

  GameEnd* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const GameEnd& from);
  void MergeFrom(const GameEnd& from);
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

  // repeated .proto.game.nanpingmahjonggame.JinZhongGangInfo jinzhonggang = 1;
  inline int jinzhonggang_size() const;
  inline void clear_jinzhonggang();
  static const int kJinzhonggangFieldNumber = 1;
  inline const ::proto::game::nanpingmahjonggame::JinZhongGangInfo& jinzhonggang(int index) const;
  inline ::proto::game::nanpingmahjonggame::JinZhongGangInfo* mutable_jinzhonggang(int index);
  inline ::proto::game::nanpingmahjonggame::JinZhongGangInfo* add_jinzhonggang();
  inline const ::google::protobuf::RepeatedPtrField< ::proto::game::nanpingmahjonggame::JinZhongGangInfo >&
      jinzhonggang() const;
  inline ::google::protobuf::RepeatedPtrField< ::proto::game::nanpingmahjonggame::JinZhongGangInfo >*
      mutable_jinzhonggang();

  // required int32 jzh_times = 2;
  inline bool has_jzh_times() const;
  inline void clear_jzh_times();
  static const int kJzhTimesFieldNumber = 2;
  inline ::google::protobuf::int32 jzh_times() const;
  inline void set_jzh_times(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:proto.game.nanpingmahjonggame.GameEnd)
 private:
  inline void set_has_jzh_times();
  inline void clear_has_jzh_times();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::RepeatedPtrField< ::proto::game::nanpingmahjonggame::JinZhongGangInfo > jinzhonggang_;
  ::google::protobuf::int32 jzh_times_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];

  friend void  protobuf_AddDesc_nanpingmahjonggame_2eproto();
  friend void protobuf_AssignDesc_nanpingmahjonggame_2eproto();
  friend void protobuf_ShutdownFile_nanpingmahjonggame_2eproto();

  void InitAsDefaultInstance();
  static GameEnd* default_instance_;
};
// -------------------------------------------------------------------

class ZhongSum : public ::google::protobuf::Message {
 public:
  ZhongSum();
  virtual ~ZhongSum();

  ZhongSum(const ZhongSum& from);

  inline ZhongSum& operator=(const ZhongSum& from) {
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
  static const ZhongSum& default_instance();

  void Swap(ZhongSum* other);

  // implements Message ----------------------------------------------

  ZhongSum* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ZhongSum& from);
  void MergeFrom(const ZhongSum& from);
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

  // repeated int32 zhong_sum = 1;
  inline int zhong_sum_size() const;
  inline void clear_zhong_sum();
  static const int kZhongSumFieldNumber = 1;
  inline ::google::protobuf::int32 zhong_sum(int index) const;
  inline void set_zhong_sum(int index, ::google::protobuf::int32 value);
  inline void add_zhong_sum(::google::protobuf::int32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
      zhong_sum() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
      mutable_zhong_sum();

  // @@protoc_insertion_point(class_scope:proto.game.nanpingmahjonggame.ZhongSum)
 private:

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::RepeatedField< ::google::protobuf::int32 > zhong_sum_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];

  friend void  protobuf_AddDesc_nanpingmahjonggame_2eproto();
  friend void protobuf_AssignDesc_nanpingmahjonggame_2eproto();
  friend void protobuf_ShutdownFile_nanpingmahjonggame_2eproto();

  void InitAsDefaultInstance();
  static ZhongSum* default_instance_;
};
// -------------------------------------------------------------------

class NanPingGameSence : public ::google::protobuf::Message {
 public:
  NanPingGameSence();
  virtual ~NanPingGameSence();

  NanPingGameSence(const NanPingGameSence& from);

  inline NanPingGameSence& operator=(const NanPingGameSence& from) {
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
  static const NanPingGameSence& default_instance();

  void Swap(NanPingGameSence* other);

  // implements Message ----------------------------------------------

  NanPingGameSence* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const NanPingGameSence& from);
  void MergeFrom(const NanPingGameSence& from);
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

  // required .proto.game.nanpingmahjonggame.ZhongSum zhong_sum = 1;
  inline bool has_zhong_sum() const;
  inline void clear_zhong_sum();
  static const int kZhongSumFieldNumber = 1;
  inline const ::proto::game::nanpingmahjonggame::ZhongSum& zhong_sum() const;
  inline ::proto::game::nanpingmahjonggame::ZhongSum* mutable_zhong_sum();
  inline ::proto::game::nanpingmahjonggame::ZhongSum* release_zhong_sum();
  inline void set_allocated_zhong_sum(::proto::game::nanpingmahjonggame::ZhongSum* zhong_sum);

  // @@protoc_insertion_point(class_scope:proto.game.nanpingmahjonggame.NanPingGameSence)
 private:
  inline void set_has_zhong_sum();
  inline void clear_has_zhong_sum();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::proto::game::nanpingmahjonggame::ZhongSum* zhong_sum_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];

  friend void  protobuf_AddDesc_nanpingmahjonggame_2eproto();
  friend void protobuf_AssignDesc_nanpingmahjonggame_2eproto();
  friend void protobuf_ShutdownFile_nanpingmahjonggame_2eproto();

  void InitAsDefaultInstance();
  static NanPingGameSence* default_instance_;
};
// ===================================================================


// ===================================================================

// Info

// repeated int32 card = 2;
inline int Info::card_size() const {
  return card_.size();
}
inline void Info::clear_card() {
  card_.Clear();
}
inline ::google::protobuf::int32 Info::card(int index) const {
  return card_.Get(index);
}
inline void Info::set_card(int index, ::google::protobuf::int32 value) {
  card_.Set(index, value);
}
inline void Info::add_card(::google::protobuf::int32 value) {
  card_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
Info::card() const {
  return card_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
Info::mutable_card() {
  return &card_;
}

// repeated int32 card_num = 3;
inline int Info::card_num_size() const {
  return card_num_.size();
}
inline void Info::clear_card_num() {
  card_num_.Clear();
}
inline ::google::protobuf::int32 Info::card_num(int index) const {
  return card_num_.Get(index);
}
inline void Info::set_card_num(int index, ::google::protobuf::int32 value) {
  card_num_.Set(index, value);
}
inline void Info::add_card_num(::google::protobuf::int32 value) {
  card_num_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
Info::card_num() const {
  return card_num_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
Info::mutable_card_num() {
  return &card_num_;
}

// -------------------------------------------------------------------

// JinZhongGangInfo

// required int32 sum = 1;
inline bool JinZhongGangInfo::has_sum() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void JinZhongGangInfo::set_has_sum() {
  _has_bits_[0] |= 0x00000001u;
}
inline void JinZhongGangInfo::clear_has_sum() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void JinZhongGangInfo::clear_sum() {
  sum_ = 0;
  clear_has_sum();
}
inline ::google::protobuf::int32 JinZhongGangInfo::sum() const {
  return sum_;
}
inline void JinZhongGangInfo::set_sum(::google::protobuf::int32 value) {
  set_has_sum();
  sum_ = value;
}

// repeated .proto.game.nanpingmahjonggame.Info info = 2;
inline int JinZhongGangInfo::info_size() const {
  return info_.size();
}
inline void JinZhongGangInfo::clear_info() {
  info_.Clear();
}
inline const ::proto::game::nanpingmahjonggame::Info& JinZhongGangInfo::info(int index) const {
  return info_.Get(index);
}
inline ::proto::game::nanpingmahjonggame::Info* JinZhongGangInfo::mutable_info(int index) {
  return info_.Mutable(index);
}
inline ::proto::game::nanpingmahjonggame::Info* JinZhongGangInfo::add_info() {
  return info_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::proto::game::nanpingmahjonggame::Info >&
JinZhongGangInfo::info() const {
  return info_;
}
inline ::google::protobuf::RepeatedPtrField< ::proto::game::nanpingmahjonggame::Info >*
JinZhongGangInfo::mutable_info() {
  return &info_;
}

// -------------------------------------------------------------------

// GameEnd

// repeated .proto.game.nanpingmahjonggame.JinZhongGangInfo jinzhonggang = 1;
inline int GameEnd::jinzhonggang_size() const {
  return jinzhonggang_.size();
}
inline void GameEnd::clear_jinzhonggang() {
  jinzhonggang_.Clear();
}
inline const ::proto::game::nanpingmahjonggame::JinZhongGangInfo& GameEnd::jinzhonggang(int index) const {
  return jinzhonggang_.Get(index);
}
inline ::proto::game::nanpingmahjonggame::JinZhongGangInfo* GameEnd::mutable_jinzhonggang(int index) {
  return jinzhonggang_.Mutable(index);
}
inline ::proto::game::nanpingmahjonggame::JinZhongGangInfo* GameEnd::add_jinzhonggang() {
  return jinzhonggang_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::proto::game::nanpingmahjonggame::JinZhongGangInfo >&
GameEnd::jinzhonggang() const {
  return jinzhonggang_;
}
inline ::google::protobuf::RepeatedPtrField< ::proto::game::nanpingmahjonggame::JinZhongGangInfo >*
GameEnd::mutable_jinzhonggang() {
  return &jinzhonggang_;
}

// required int32 jzh_times = 2;
inline bool GameEnd::has_jzh_times() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void GameEnd::set_has_jzh_times() {
  _has_bits_[0] |= 0x00000002u;
}
inline void GameEnd::clear_has_jzh_times() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void GameEnd::clear_jzh_times() {
  jzh_times_ = 0;
  clear_has_jzh_times();
}
inline ::google::protobuf::int32 GameEnd::jzh_times() const {
  return jzh_times_;
}
inline void GameEnd::set_jzh_times(::google::protobuf::int32 value) {
  set_has_jzh_times();
  jzh_times_ = value;
}

// -------------------------------------------------------------------

// ZhongSum

// repeated int32 zhong_sum = 1;
inline int ZhongSum::zhong_sum_size() const {
  return zhong_sum_.size();
}
inline void ZhongSum::clear_zhong_sum() {
  zhong_sum_.Clear();
}
inline ::google::protobuf::int32 ZhongSum::zhong_sum(int index) const {
  return zhong_sum_.Get(index);
}
inline void ZhongSum::set_zhong_sum(int index, ::google::protobuf::int32 value) {
  zhong_sum_.Set(index, value);
}
inline void ZhongSum::add_zhong_sum(::google::protobuf::int32 value) {
  zhong_sum_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
ZhongSum::zhong_sum() const {
  return zhong_sum_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
ZhongSum::mutable_zhong_sum() {
  return &zhong_sum_;
}

// -------------------------------------------------------------------

// NanPingGameSence

// required .proto.game.nanpingmahjonggame.ZhongSum zhong_sum = 1;
inline bool NanPingGameSence::has_zhong_sum() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void NanPingGameSence::set_has_zhong_sum() {
  _has_bits_[0] |= 0x00000001u;
}
inline void NanPingGameSence::clear_has_zhong_sum() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void NanPingGameSence::clear_zhong_sum() {
  if (zhong_sum_ != NULL) zhong_sum_->::proto::game::nanpingmahjonggame::ZhongSum::Clear();
  clear_has_zhong_sum();
}
inline const ::proto::game::nanpingmahjonggame::ZhongSum& NanPingGameSence::zhong_sum() const {
  return zhong_sum_ != NULL ? *zhong_sum_ : *default_instance_->zhong_sum_;
}
inline ::proto::game::nanpingmahjonggame::ZhongSum* NanPingGameSence::mutable_zhong_sum() {
  set_has_zhong_sum();
  if (zhong_sum_ == NULL) zhong_sum_ = new ::proto::game::nanpingmahjonggame::ZhongSum;
  return zhong_sum_;
}
inline ::proto::game::nanpingmahjonggame::ZhongSum* NanPingGameSence::release_zhong_sum() {
  clear_has_zhong_sum();
  ::proto::game::nanpingmahjonggame::ZhongSum* temp = zhong_sum_;
  zhong_sum_ = NULL;
  return temp;
}
inline void NanPingGameSence::set_allocated_zhong_sum(::proto::game::nanpingmahjonggame::ZhongSum* zhong_sum) {
  delete zhong_sum_;
  zhong_sum_ = zhong_sum;
  if (zhong_sum) {
    set_has_zhong_sum();
  } else {
    clear_has_zhong_sum();
  }
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace nanpingmahjonggame
}  // namespace game
}  // namespace proto

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_nanpingmahjonggame_2eproto__INCLUDED
