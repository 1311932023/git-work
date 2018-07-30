// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: anqinggameproto.proto

#ifndef PROTOBUF_anqinggameproto_2eproto__INCLUDED
#define PROTOBUF_anqinggameproto_2eproto__INCLUDED

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
namespace anqingmj {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_anqinggameproto_2eproto();
void protobuf_AssignDesc_anqinggameproto_2eproto();
void protobuf_ShutdownFile_anqinggameproto_2eproto();

class AnQingGameEnd;

// ===================================================================

class AnQingGameEnd : public ::google::protobuf::Message {
 public:
  AnQingGameEnd();
  virtual ~AnQingGameEnd();

  AnQingGameEnd(const AnQingGameEnd& from);

  inline AnQingGameEnd& operator=(const AnQingGameEnd& from) {
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
  static const AnQingGameEnd& default_instance();

  void Swap(AnQingGameEnd* other);

  // implements Message ----------------------------------------------

  AnQingGameEnd* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const AnQingGameEnd& from);
  void MergeFrom(const AnQingGameEnd& from);
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

  // repeated int32 BuHuaNum = 1;
  inline int buhuanum_size() const;
  inline void clear_buhuanum();
  static const int kBuHuaNumFieldNumber = 1;
  inline ::google::protobuf::int32 buhuanum(int index) const;
  inline void set_buhuanum(int index, ::google::protobuf::int32 value);
  inline void add_buhuanum(::google::protobuf::int32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
      buhuanum() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
      mutable_buhuanum();

  // @@protoc_insertion_point(class_scope:proto.game.anqingmj.AnQingGameEnd)
 private:

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::RepeatedField< ::google::protobuf::int32 > buhuanum_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];

  friend void  protobuf_AddDesc_anqinggameproto_2eproto();
  friend void protobuf_AssignDesc_anqinggameproto_2eproto();
  friend void protobuf_ShutdownFile_anqinggameproto_2eproto();

  void InitAsDefaultInstance();
  static AnQingGameEnd* default_instance_;
};
// ===================================================================


// ===================================================================

// AnQingGameEnd

// repeated int32 BuHuaNum = 1;
inline int AnQingGameEnd::buhuanum_size() const {
  return buhuanum_.size();
}
inline void AnQingGameEnd::clear_buhuanum() {
  buhuanum_.Clear();
}
inline ::google::protobuf::int32 AnQingGameEnd::buhuanum(int index) const {
  return buhuanum_.Get(index);
}
inline void AnQingGameEnd::set_buhuanum(int index, ::google::protobuf::int32 value) {
  buhuanum_.Set(index, value);
}
inline void AnQingGameEnd::add_buhuanum(::google::protobuf::int32 value) {
  buhuanum_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
AnQingGameEnd::buhuanum() const {
  return buhuanum_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
AnQingGameEnd::mutable_buhuanum() {
  return &buhuanum_;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace anqingmj
}  // namespace game
}  // namespace proto

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_anqinggameproto_2eproto__INCLUDED
