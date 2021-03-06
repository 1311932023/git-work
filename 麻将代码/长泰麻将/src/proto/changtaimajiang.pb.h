// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: changtaimajiang.proto

#ifndef PROTOBUF_changtaimajiang_2eproto__INCLUDED
#define PROTOBUF_changtaimajiang_2eproto__INCLUDED

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
namespace changtaimajiang {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_changtaimajiang_2eproto();
void protobuf_AssignDesc_changtaimajiang_2eproto();
void protobuf_ShutdownFile_changtaimajiang_2eproto();

class RspGameLaiziInfo;
class RspGameHuapaiInfo;
class RspGameHuapaiInitCardInfo;
class GameEndPlayerScoresInfo;
class RspGenZhuang;

// ===================================================================

class RspGameLaiziInfo : public ::google::protobuf::Message {
 public:
  RspGameLaiziInfo();
  virtual ~RspGameLaiziInfo();

  RspGameLaiziInfo(const RspGameLaiziInfo& from);

  inline RspGameLaiziInfo& operator=(const RspGameLaiziInfo& from) {
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
  static const RspGameLaiziInfo& default_instance();

  void Swap(RspGameLaiziInfo* other);

  // implements Message ----------------------------------------------

  RspGameLaiziInfo* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const RspGameLaiziInfo& from);
  void MergeFrom(const RspGameLaiziInfo& from);
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

  // repeated int32 laiziShowList = 1;
  inline int laizishowlist_size() const;
  inline void clear_laizishowlist();
  static const int kLaiziShowListFieldNumber = 1;
  inline ::google::protobuf::int32 laizishowlist(int index) const;
  inline void set_laizishowlist(int index, ::google::protobuf::int32 value);
  inline void add_laizishowlist(::google::protobuf::int32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
      laizishowlist() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
      mutable_laizishowlist();

  // repeated int32 laiziSignList = 2;
  inline int laizisignlist_size() const;
  inline void clear_laizisignlist();
  static const int kLaiziSignListFieldNumber = 2;
  inline ::google::protobuf::int32 laizisignlist(int index) const;
  inline void set_laizisignlist(int index, ::google::protobuf::int32 value);
  inline void add_laizisignlist(::google::protobuf::int32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
      laizisignlist() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
      mutable_laizisignlist();

  // repeated int32 diceList = 3;
  inline int dicelist_size() const;
  inline void clear_dicelist();
  static const int kDiceListFieldNumber = 3;
  inline ::google::protobuf::int32 dicelist(int index) const;
  inline void set_dicelist(int index, ::google::protobuf::int32 value);
  inline void add_dicelist(::google::protobuf::int32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
      dicelist() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
      mutable_dicelist();

  // optional bool flag = 4;
  inline bool has_flag() const;
  inline void clear_flag();
  static const int kFlagFieldNumber = 4;
  inline bool flag() const;
  inline void set_flag(bool value);

  // repeated int32 laiziReplaceList = 5;
  inline int laizireplacelist_size() const;
  inline void clear_laizireplacelist();
  static const int kLaiziReplaceListFieldNumber = 5;
  inline ::google::protobuf::int32 laizireplacelist(int index) const;
  inline void set_laizireplacelist(int index, ::google::protobuf::int32 value);
  inline void add_laizireplacelist(::google::protobuf::int32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
      laizireplacelist() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
      mutable_laizireplacelist();

  // @@protoc_insertion_point(class_scope:proto.game.changtaimajiang.RspGameLaiziInfo)
 private:
  inline void set_has_flag();
  inline void clear_has_flag();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::RepeatedField< ::google::protobuf::int32 > laizishowlist_;
  ::google::protobuf::RepeatedField< ::google::protobuf::int32 > laizisignlist_;
  ::google::protobuf::RepeatedField< ::google::protobuf::int32 > dicelist_;
  ::google::protobuf::RepeatedField< ::google::protobuf::int32 > laizireplacelist_;
  bool flag_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(5 + 31) / 32];

  friend void  protobuf_AddDesc_changtaimajiang_2eproto();
  friend void protobuf_AssignDesc_changtaimajiang_2eproto();
  friend void protobuf_ShutdownFile_changtaimajiang_2eproto();

  void InitAsDefaultInstance();
  static RspGameLaiziInfo* default_instance_;
};
// -------------------------------------------------------------------

class RspGameHuapaiInfo : public ::google::protobuf::Message {
 public:
  RspGameHuapaiInfo();
  virtual ~RspGameHuapaiInfo();

  RspGameHuapaiInfo(const RspGameHuapaiInfo& from);

  inline RspGameHuapaiInfo& operator=(const RspGameHuapaiInfo& from) {
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
  static const RspGameHuapaiInfo& default_instance();

  void Swap(RspGameHuapaiInfo* other);

  // implements Message ----------------------------------------------

  RspGameHuapaiInfo* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const RspGameHuapaiInfo& from);
  void MergeFrom(const RspGameHuapaiInfo& from);
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

  // repeated int32 huapaiList = 1;
  inline int huapailist_size() const;
  inline void clear_huapailist();
  static const int kHuapaiListFieldNumber = 1;
  inline ::google::protobuf::int32 huapailist(int index) const;
  inline void set_huapailist(int index, ::google::protobuf::int32 value);
  inline void add_huapailist(::google::protobuf::int32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
      huapailist() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
      mutable_huapailist();

  // @@protoc_insertion_point(class_scope:proto.game.changtaimajiang.RspGameHuapaiInfo)
 private:

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::RepeatedField< ::google::protobuf::int32 > huapailist_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];

  friend void  protobuf_AddDesc_changtaimajiang_2eproto();
  friend void protobuf_AssignDesc_changtaimajiang_2eproto();
  friend void protobuf_ShutdownFile_changtaimajiang_2eproto();

  void InitAsDefaultInstance();
  static RspGameHuapaiInfo* default_instance_;
};
// -------------------------------------------------------------------

class RspGameHuapaiInitCardInfo : public ::google::protobuf::Message {
 public:
  RspGameHuapaiInitCardInfo();
  virtual ~RspGameHuapaiInitCardInfo();

  RspGameHuapaiInitCardInfo(const RspGameHuapaiInitCardInfo& from);

  inline RspGameHuapaiInitCardInfo& operator=(const RspGameHuapaiInitCardInfo& from) {
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
  static const RspGameHuapaiInitCardInfo& default_instance();

  void Swap(RspGameHuapaiInitCardInfo* other);

  // implements Message ----------------------------------------------

  RspGameHuapaiInitCardInfo* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const RspGameHuapaiInitCardInfo& from);
  void MergeFrom(const RspGameHuapaiInitCardInfo& from);
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

  // optional int32 seatid = 1;
  inline bool has_seatid() const;
  inline void clear_seatid();
  static const int kSeatidFieldNumber = 1;
  inline ::google::protobuf::int32 seatid() const;
  inline void set_seatid(::google::protobuf::int32 value);

  // repeated int32 cards = 2;
  inline int cards_size() const;
  inline void clear_cards();
  static const int kCardsFieldNumber = 2;
  inline ::google::protobuf::int32 cards(int index) const;
  inline void set_cards(int index, ::google::protobuf::int32 value);
  inline void add_cards(::google::protobuf::int32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
      cards() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
      mutable_cards();

  // @@protoc_insertion_point(class_scope:proto.game.changtaimajiang.RspGameHuapaiInitCardInfo)
 private:
  inline void set_has_seatid();
  inline void clear_has_seatid();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::RepeatedField< ::google::protobuf::int32 > cards_;
  ::google::protobuf::int32 seatid_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];

  friend void  protobuf_AddDesc_changtaimajiang_2eproto();
  friend void protobuf_AssignDesc_changtaimajiang_2eproto();
  friend void protobuf_ShutdownFile_changtaimajiang_2eproto();

  void InitAsDefaultInstance();
  static RspGameHuapaiInitCardInfo* default_instance_;
};
// -------------------------------------------------------------------

class GameEndPlayerScoresInfo : public ::google::protobuf::Message {
 public:
  GameEndPlayerScoresInfo();
  virtual ~GameEndPlayerScoresInfo();

  GameEndPlayerScoresInfo(const GameEndPlayerScoresInfo& from);

  inline GameEndPlayerScoresInfo& operator=(const GameEndPlayerScoresInfo& from) {
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
  static const GameEndPlayerScoresInfo& default_instance();

  void Swap(GameEndPlayerScoresInfo* other);

  // implements Message ----------------------------------------------

  GameEndPlayerScoresInfo* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const GameEndPlayerScoresInfo& from);
  void MergeFrom(const GameEndPlayerScoresInfo& from);
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

  // optional int32 seatid = 1;
  inline bool has_seatid() const;
  inline void clear_seatid();
  static const int kSeatidFieldNumber = 1;
  inline ::google::protobuf::int32 seatid() const;
  inline void set_seatid(::google::protobuf::int32 value);

  // optional int32 gangscore = 2;
  inline bool has_gangscore() const;
  inline void clear_gangscore();
  static const int kGangscoreFieldNumber = 2;
  inline ::google::protobuf::int32 gangscore() const;
  inline void set_gangscore(::google::protobuf::int32 value);

  // optional int32 huscore = 3;
  inline bool has_huscore() const;
  inline void clear_huscore();
  static const int kHuscoreFieldNumber = 3;
  inline ::google::protobuf::int32 huscore() const;
  inline void set_huscore(::google::protobuf::int32 value);

  // optional int32 dianscroe = 4;
  inline bool has_dianscroe() const;
  inline void clear_dianscroe();
  static const int kDianscroeFieldNumber = 4;
  inline ::google::protobuf::int32 dianscroe() const;
  inline void set_dianscroe(::google::protobuf::int32 value);

  // optional int32 fanscore = 5;
  inline bool has_fanscore() const;
  inline void clear_fanscore();
  static const int kFanscoreFieldNumber = 5;
  inline ::google::protobuf::int32 fanscore() const;
  inline void set_fanscore(::google::protobuf::int32 value);

  // optional int32 zengscore = 6;
  inline bool has_zengscore() const;
  inline void clear_zengscore();
  static const int kZengscoreFieldNumber = 6;
  inline ::google::protobuf::int32 zengscore() const;
  inline void set_zengscore(::google::protobuf::int32 value);

  // optional int32 gangshangpaoscore = 7;
  inline bool has_gangshangpaoscore() const;
  inline void clear_gangshangpaoscore();
  static const int kGangshangpaoscoreFieldNumber = 7;
  inline ::google::protobuf::int32 gangshangpaoscore() const;
  inline void set_gangshangpaoscore(::google::protobuf::int32 value);

  // optional int32 extra_dianscore = 8;
  inline bool has_extra_dianscore() const;
  inline void clear_extra_dianscore();
  static const int kExtraDianscoreFieldNumber = 8;
  inline ::google::protobuf::int32 extra_dianscore() const;
  inline void set_extra_dianscore(::google::protobuf::int32 value);

  // optional int32 total_dianscore = 9;
  inline bool has_total_dianscore() const;
  inline void clear_total_dianscore();
  static const int kTotalDianscoreFieldNumber = 9;
  inline ::google::protobuf::int32 total_dianscore() const;
  inline void set_total_dianscore(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:proto.game.changtaimajiang.GameEndPlayerScoresInfo)
 private:
  inline void set_has_seatid();
  inline void clear_has_seatid();
  inline void set_has_gangscore();
  inline void clear_has_gangscore();
  inline void set_has_huscore();
  inline void clear_has_huscore();
  inline void set_has_dianscroe();
  inline void clear_has_dianscroe();
  inline void set_has_fanscore();
  inline void clear_has_fanscore();
  inline void set_has_zengscore();
  inline void clear_has_zengscore();
  inline void set_has_gangshangpaoscore();
  inline void clear_has_gangshangpaoscore();
  inline void set_has_extra_dianscore();
  inline void clear_has_extra_dianscore();
  inline void set_has_total_dianscore();
  inline void clear_has_total_dianscore();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::int32 seatid_;
  ::google::protobuf::int32 gangscore_;
  ::google::protobuf::int32 huscore_;
  ::google::protobuf::int32 dianscroe_;
  ::google::protobuf::int32 fanscore_;
  ::google::protobuf::int32 zengscore_;
  ::google::protobuf::int32 gangshangpaoscore_;
  ::google::protobuf::int32 extra_dianscore_;
  ::google::protobuf::int32 total_dianscore_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(9 + 31) / 32];

  friend void  protobuf_AddDesc_changtaimajiang_2eproto();
  friend void protobuf_AssignDesc_changtaimajiang_2eproto();
  friend void protobuf_ShutdownFile_changtaimajiang_2eproto();

  void InitAsDefaultInstance();
  static GameEndPlayerScoresInfo* default_instance_;
};
// -------------------------------------------------------------------

class RspGenZhuang : public ::google::protobuf::Message {
 public:
  RspGenZhuang();
  virtual ~RspGenZhuang();

  RspGenZhuang(const RspGenZhuang& from);

  inline RspGenZhuang& operator=(const RspGenZhuang& from) {
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
  static const RspGenZhuang& default_instance();

  void Swap(RspGenZhuang* other);

  // implements Message ----------------------------------------------

  RspGenZhuang* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const RspGenZhuang& from);
  void MergeFrom(const RspGenZhuang& from);
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

  // optional bool gengzhaungflag = 1;
  inline bool has_gengzhaungflag() const;
  inline void clear_gengzhaungflag();
  static const int kGengzhaungflagFieldNumber = 1;
  inline bool gengzhaungflag() const;
  inline void set_gengzhaungflag(bool value);

  // @@protoc_insertion_point(class_scope:proto.game.changtaimajiang.RspGenZhuang)
 private:
  inline void set_has_gengzhaungflag();
  inline void clear_has_gengzhaungflag();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  bool gengzhaungflag_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];

  friend void  protobuf_AddDesc_changtaimajiang_2eproto();
  friend void protobuf_AssignDesc_changtaimajiang_2eproto();
  friend void protobuf_ShutdownFile_changtaimajiang_2eproto();

  void InitAsDefaultInstance();
  static RspGenZhuang* default_instance_;
};
// ===================================================================


// ===================================================================

// RspGameLaiziInfo

// repeated int32 laiziShowList = 1;
inline int RspGameLaiziInfo::laizishowlist_size() const {
  return laizishowlist_.size();
}
inline void RspGameLaiziInfo::clear_laizishowlist() {
  laizishowlist_.Clear();
}
inline ::google::protobuf::int32 RspGameLaiziInfo::laizishowlist(int index) const {
  return laizishowlist_.Get(index);
}
inline void RspGameLaiziInfo::set_laizishowlist(int index, ::google::protobuf::int32 value) {
  laizishowlist_.Set(index, value);
}
inline void RspGameLaiziInfo::add_laizishowlist(::google::protobuf::int32 value) {
  laizishowlist_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
RspGameLaiziInfo::laizishowlist() const {
  return laizishowlist_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
RspGameLaiziInfo::mutable_laizishowlist() {
  return &laizishowlist_;
}

// repeated int32 laiziSignList = 2;
inline int RspGameLaiziInfo::laizisignlist_size() const {
  return laizisignlist_.size();
}
inline void RspGameLaiziInfo::clear_laizisignlist() {
  laizisignlist_.Clear();
}
inline ::google::protobuf::int32 RspGameLaiziInfo::laizisignlist(int index) const {
  return laizisignlist_.Get(index);
}
inline void RspGameLaiziInfo::set_laizisignlist(int index, ::google::protobuf::int32 value) {
  laizisignlist_.Set(index, value);
}
inline void RspGameLaiziInfo::add_laizisignlist(::google::protobuf::int32 value) {
  laizisignlist_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
RspGameLaiziInfo::laizisignlist() const {
  return laizisignlist_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
RspGameLaiziInfo::mutable_laizisignlist() {
  return &laizisignlist_;
}

// repeated int32 diceList = 3;
inline int RspGameLaiziInfo::dicelist_size() const {
  return dicelist_.size();
}
inline void RspGameLaiziInfo::clear_dicelist() {
  dicelist_.Clear();
}
inline ::google::protobuf::int32 RspGameLaiziInfo::dicelist(int index) const {
  return dicelist_.Get(index);
}
inline void RspGameLaiziInfo::set_dicelist(int index, ::google::protobuf::int32 value) {
  dicelist_.Set(index, value);
}
inline void RspGameLaiziInfo::add_dicelist(::google::protobuf::int32 value) {
  dicelist_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
RspGameLaiziInfo::dicelist() const {
  return dicelist_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
RspGameLaiziInfo::mutable_dicelist() {
  return &dicelist_;
}

// optional bool flag = 4;
inline bool RspGameLaiziInfo::has_flag() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void RspGameLaiziInfo::set_has_flag() {
  _has_bits_[0] |= 0x00000008u;
}
inline void RspGameLaiziInfo::clear_has_flag() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void RspGameLaiziInfo::clear_flag() {
  flag_ = false;
  clear_has_flag();
}
inline bool RspGameLaiziInfo::flag() const {
  return flag_;
}
inline void RspGameLaiziInfo::set_flag(bool value) {
  set_has_flag();
  flag_ = value;
}

// repeated int32 laiziReplaceList = 5;
inline int RspGameLaiziInfo::laizireplacelist_size() const {
  return laizireplacelist_.size();
}
inline void RspGameLaiziInfo::clear_laizireplacelist() {
  laizireplacelist_.Clear();
}
inline ::google::protobuf::int32 RspGameLaiziInfo::laizireplacelist(int index) const {
  return laizireplacelist_.Get(index);
}
inline void RspGameLaiziInfo::set_laizireplacelist(int index, ::google::protobuf::int32 value) {
  laizireplacelist_.Set(index, value);
}
inline void RspGameLaiziInfo::add_laizireplacelist(::google::protobuf::int32 value) {
  laizireplacelist_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
RspGameLaiziInfo::laizireplacelist() const {
  return laizireplacelist_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
RspGameLaiziInfo::mutable_laizireplacelist() {
  return &laizireplacelist_;
}

// -------------------------------------------------------------------

// RspGameHuapaiInfo

// repeated int32 huapaiList = 1;
inline int RspGameHuapaiInfo::huapailist_size() const {
  return huapailist_.size();
}
inline void RspGameHuapaiInfo::clear_huapailist() {
  huapailist_.Clear();
}
inline ::google::protobuf::int32 RspGameHuapaiInfo::huapailist(int index) const {
  return huapailist_.Get(index);
}
inline void RspGameHuapaiInfo::set_huapailist(int index, ::google::protobuf::int32 value) {
  huapailist_.Set(index, value);
}
inline void RspGameHuapaiInfo::add_huapailist(::google::protobuf::int32 value) {
  huapailist_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
RspGameHuapaiInfo::huapailist() const {
  return huapailist_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
RspGameHuapaiInfo::mutable_huapailist() {
  return &huapailist_;
}

// -------------------------------------------------------------------

// RspGameHuapaiInitCardInfo

// optional int32 seatid = 1;
inline bool RspGameHuapaiInitCardInfo::has_seatid() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void RspGameHuapaiInitCardInfo::set_has_seatid() {
  _has_bits_[0] |= 0x00000001u;
}
inline void RspGameHuapaiInitCardInfo::clear_has_seatid() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void RspGameHuapaiInitCardInfo::clear_seatid() {
  seatid_ = 0;
  clear_has_seatid();
}
inline ::google::protobuf::int32 RspGameHuapaiInitCardInfo::seatid() const {
  return seatid_;
}
inline void RspGameHuapaiInitCardInfo::set_seatid(::google::protobuf::int32 value) {
  set_has_seatid();
  seatid_ = value;
}

// repeated int32 cards = 2;
inline int RspGameHuapaiInitCardInfo::cards_size() const {
  return cards_.size();
}
inline void RspGameHuapaiInitCardInfo::clear_cards() {
  cards_.Clear();
}
inline ::google::protobuf::int32 RspGameHuapaiInitCardInfo::cards(int index) const {
  return cards_.Get(index);
}
inline void RspGameHuapaiInitCardInfo::set_cards(int index, ::google::protobuf::int32 value) {
  cards_.Set(index, value);
}
inline void RspGameHuapaiInitCardInfo::add_cards(::google::protobuf::int32 value) {
  cards_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
RspGameHuapaiInitCardInfo::cards() const {
  return cards_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
RspGameHuapaiInitCardInfo::mutable_cards() {
  return &cards_;
}

// -------------------------------------------------------------------

// GameEndPlayerScoresInfo

// optional int32 seatid = 1;
inline bool GameEndPlayerScoresInfo::has_seatid() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void GameEndPlayerScoresInfo::set_has_seatid() {
  _has_bits_[0] |= 0x00000001u;
}
inline void GameEndPlayerScoresInfo::clear_has_seatid() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void GameEndPlayerScoresInfo::clear_seatid() {
  seatid_ = 0;
  clear_has_seatid();
}
inline ::google::protobuf::int32 GameEndPlayerScoresInfo::seatid() const {
  return seatid_;
}
inline void GameEndPlayerScoresInfo::set_seatid(::google::protobuf::int32 value) {
  set_has_seatid();
  seatid_ = value;
}

// optional int32 gangscore = 2;
inline bool GameEndPlayerScoresInfo::has_gangscore() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void GameEndPlayerScoresInfo::set_has_gangscore() {
  _has_bits_[0] |= 0x00000002u;
}
inline void GameEndPlayerScoresInfo::clear_has_gangscore() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void GameEndPlayerScoresInfo::clear_gangscore() {
  gangscore_ = 0;
  clear_has_gangscore();
}
inline ::google::protobuf::int32 GameEndPlayerScoresInfo::gangscore() const {
  return gangscore_;
}
inline void GameEndPlayerScoresInfo::set_gangscore(::google::protobuf::int32 value) {
  set_has_gangscore();
  gangscore_ = value;
}

// optional int32 huscore = 3;
inline bool GameEndPlayerScoresInfo::has_huscore() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void GameEndPlayerScoresInfo::set_has_huscore() {
  _has_bits_[0] |= 0x00000004u;
}
inline void GameEndPlayerScoresInfo::clear_has_huscore() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void GameEndPlayerScoresInfo::clear_huscore() {
  huscore_ = 0;
  clear_has_huscore();
}
inline ::google::protobuf::int32 GameEndPlayerScoresInfo::huscore() const {
  return huscore_;
}
inline void GameEndPlayerScoresInfo::set_huscore(::google::protobuf::int32 value) {
  set_has_huscore();
  huscore_ = value;
}

// optional int32 dianscroe = 4;
inline bool GameEndPlayerScoresInfo::has_dianscroe() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void GameEndPlayerScoresInfo::set_has_dianscroe() {
  _has_bits_[0] |= 0x00000008u;
}
inline void GameEndPlayerScoresInfo::clear_has_dianscroe() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void GameEndPlayerScoresInfo::clear_dianscroe() {
  dianscroe_ = 0;
  clear_has_dianscroe();
}
inline ::google::protobuf::int32 GameEndPlayerScoresInfo::dianscroe() const {
  return dianscroe_;
}
inline void GameEndPlayerScoresInfo::set_dianscroe(::google::protobuf::int32 value) {
  set_has_dianscroe();
  dianscroe_ = value;
}

// optional int32 fanscore = 5;
inline bool GameEndPlayerScoresInfo::has_fanscore() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void GameEndPlayerScoresInfo::set_has_fanscore() {
  _has_bits_[0] |= 0x00000010u;
}
inline void GameEndPlayerScoresInfo::clear_has_fanscore() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void GameEndPlayerScoresInfo::clear_fanscore() {
  fanscore_ = 0;
  clear_has_fanscore();
}
inline ::google::protobuf::int32 GameEndPlayerScoresInfo::fanscore() const {
  return fanscore_;
}
inline void GameEndPlayerScoresInfo::set_fanscore(::google::protobuf::int32 value) {
  set_has_fanscore();
  fanscore_ = value;
}

// optional int32 zengscore = 6;
inline bool GameEndPlayerScoresInfo::has_zengscore() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void GameEndPlayerScoresInfo::set_has_zengscore() {
  _has_bits_[0] |= 0x00000020u;
}
inline void GameEndPlayerScoresInfo::clear_has_zengscore() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void GameEndPlayerScoresInfo::clear_zengscore() {
  zengscore_ = 0;
  clear_has_zengscore();
}
inline ::google::protobuf::int32 GameEndPlayerScoresInfo::zengscore() const {
  return zengscore_;
}
inline void GameEndPlayerScoresInfo::set_zengscore(::google::protobuf::int32 value) {
  set_has_zengscore();
  zengscore_ = value;
}

// optional int32 gangshangpaoscore = 7;
inline bool GameEndPlayerScoresInfo::has_gangshangpaoscore() const {
  return (_has_bits_[0] & 0x00000040u) != 0;
}
inline void GameEndPlayerScoresInfo::set_has_gangshangpaoscore() {
  _has_bits_[0] |= 0x00000040u;
}
inline void GameEndPlayerScoresInfo::clear_has_gangshangpaoscore() {
  _has_bits_[0] &= ~0x00000040u;
}
inline void GameEndPlayerScoresInfo::clear_gangshangpaoscore() {
  gangshangpaoscore_ = 0;
  clear_has_gangshangpaoscore();
}
inline ::google::protobuf::int32 GameEndPlayerScoresInfo::gangshangpaoscore() const {
  return gangshangpaoscore_;
}
inline void GameEndPlayerScoresInfo::set_gangshangpaoscore(::google::protobuf::int32 value) {
  set_has_gangshangpaoscore();
  gangshangpaoscore_ = value;
}

// optional int32 extra_dianscore = 8;
inline bool GameEndPlayerScoresInfo::has_extra_dianscore() const {
  return (_has_bits_[0] & 0x00000080u) != 0;
}
inline void GameEndPlayerScoresInfo::set_has_extra_dianscore() {
  _has_bits_[0] |= 0x00000080u;
}
inline void GameEndPlayerScoresInfo::clear_has_extra_dianscore() {
  _has_bits_[0] &= ~0x00000080u;
}
inline void GameEndPlayerScoresInfo::clear_extra_dianscore() {
  extra_dianscore_ = 0;
  clear_has_extra_dianscore();
}
inline ::google::protobuf::int32 GameEndPlayerScoresInfo::extra_dianscore() const {
  return extra_dianscore_;
}
inline void GameEndPlayerScoresInfo::set_extra_dianscore(::google::protobuf::int32 value) {
  set_has_extra_dianscore();
  extra_dianscore_ = value;
}

// optional int32 total_dianscore = 9;
inline bool GameEndPlayerScoresInfo::has_total_dianscore() const {
  return (_has_bits_[0] & 0x00000100u) != 0;
}
inline void GameEndPlayerScoresInfo::set_has_total_dianscore() {
  _has_bits_[0] |= 0x00000100u;
}
inline void GameEndPlayerScoresInfo::clear_has_total_dianscore() {
  _has_bits_[0] &= ~0x00000100u;
}
inline void GameEndPlayerScoresInfo::clear_total_dianscore() {
  total_dianscore_ = 0;
  clear_has_total_dianscore();
}
inline ::google::protobuf::int32 GameEndPlayerScoresInfo::total_dianscore() const {
  return total_dianscore_;
}
inline void GameEndPlayerScoresInfo::set_total_dianscore(::google::protobuf::int32 value) {
  set_has_total_dianscore();
  total_dianscore_ = value;
}

// -------------------------------------------------------------------

// RspGenZhuang

// optional bool gengzhaungflag = 1;
inline bool RspGenZhuang::has_gengzhaungflag() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void RspGenZhuang::set_has_gengzhaungflag() {
  _has_bits_[0] |= 0x00000001u;
}
inline void RspGenZhuang::clear_has_gengzhaungflag() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void RspGenZhuang::clear_gengzhaungflag() {
  gengzhaungflag_ = false;
  clear_has_gengzhaungflag();
}
inline bool RspGenZhuang::gengzhaungflag() const {
  return gengzhaungflag_;
}
inline void RspGenZhuang::set_gengzhaungflag(bool value) {
  set_has_gengzhaungflag();
  gengzhaungflag_ = value;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace changtaimajiang
}  // namespace game
}  // namespace proto

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_changtaimajiang_2eproto__INCLUDED
