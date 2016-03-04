/*
 *  Copyright (c) 2014-2016 olibre (olibre@Lmap.org)
 *
 *  This file is part of the library pbart (Protocol Buffers At RunTime).
 *
 *  pbart is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License (AGPL)
 *  version 3 or (at your option) any later version.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with pbart (maybe in file LICENSE).
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#include "FirstInclude.h"
#include "pbart/Codec.h"
#include "pbart/Serializer.h"
#include "macros.h"

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4996)
#endif
#include <google/protobuf/wire_format_lite_inl.h>
#ifdef _MSC_VER
#  pragma warning(pop)
#endif

#include <utility> //pair<>


namespace pbart
{

//--------------------------------------------------------------------------
inline std::string Codec::header (InputStream& input, gp::uint32 tag, Id id)
{
    // extract the wire type from tag
    gpi::WireFormatLite::WireType wt = gpi::WireFormatLite::GetTagWireType (tag);
    // check wire type consistency
    if (wt != gpi::WireFormatLite::WIRETYPE_LENGTH_DELIMITED)
    {
        throw MsgTypeException (id, wt, gpi::WireFormatLite::WIRETYPE_LENGTH_DELIMITED);
    }

    std::string data;
    bool ok = gpi::WireFormatLite::ReadBytes (&input, &data);
    if (!ok)
    {
        throw DecodeException (id, "Cannot decode header within Message frame");
    }

    return data;
}

//  //--------------------------------------------------------------------------
//  // not used
//  // Id Codec::id (InputStream& input, const std::string& h)
//  {
//    gp::uint32  tag  = input.ReadTag();
//    Id          id   = gpi::WireFormatLite::GetTagFieldNumber(tag);
//    std::string data = header (input, tag, id);
//
//    if (data != h)
//    {
//      throw DecodeException (id, h, data); //TODO(Oliver): try to reload again dico (XML file)
//    }
//
//    return id;
//  }

//--------------------------------------------------------------------------
// helper functions for decoding
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
template< gpi::WireFormatLite::WireType  wireType,
          gpi::WireFormatLite::FieldType fieldType,
          typename                       valueType >
inline valueType readAny( Id                  id,
                          Id                  item,
                          gp::uint32          tag,
                          Codec::InputStream& input )
{
    // extract the wire type from tag
    gpi::WireFormatLite::WireType wt =
            gpi::WireFormatLite::GetTagWireType (tag);

    // check wire type consistency
    if (wt != wireType)
        throw MsgTypeException (id, item, wt, wireType);

    // read and convert the value (decode)
    valueType value;
    bool ok = gpi::WireFormatLite::ReadPrimitive <valueType, fieldType> (&input, &value);
    if (!ok)
        throw DecodeException (id, item, fieldType);

    return value;
}
//--------------------------------------------------------------------------
static inline
bool readString (std::string& value, Codec::InputStream& input)
{
#ifdef USE_STRICT_BOOST_UTF8_STRING_FORMAT
    bool ok = gpi::WireFormatLite::ReadString(&input, &value); //theorically should return false on UTF-8 incosistency
    if ( ok )
        gpi::WireFormat::VerifyUTF8String (value.data(), value.size(), gpi::WireFormat::PARSE);
    return ok;
#else
    return gpi::WireFormatLite::ReadBytes (&input, &value); //on v2.4.1, ReadString() and ReadBytes() have same body
#endif
}
//--------------------------------------------------------------------------
static inline
void readString (Id id, Id item, std::string& value, gp::uint32 tag, Codec::InputStream& input)
{
    // extract the wire type from tag
    gpi::WireFormatLite::WireType wt =
            gpi::WireFormatLite::GetTagWireType (tag);

    // check wire type consistency
    const gpi::WireFormatLite::WireType wireType = gpi::WireFormatLite::WIRETYPE_LENGTH_DELIMITED;
    if (wt != wireType)
        throw MsgTypeException (id, item, wt, wireType);

    // read, convert and check the string (decode)
    bool ok = readString (value, input);
    if (!ok)
        throw DecodeException (id, item, "string");
}
//--------------------------------------------------------------------------
static inline
Variant::long_t readLong (Id id, Id item, gp::uint32 tag, Codec::InputStream& input)
{
    return readAny< gpi::WireFormatLite::WIRETYPE_VARINT
            , gpi::WireFormatLite::TYPE_INT64
            , Variant::long_t
            > (id, item, tag, input);
}
//--------------------------------------------------------------------------
static inline
int readLength (Id id, Id item, gp::uint32 tag, Codec::InputStream& input)
{
    return readAny< gpi::WireFormatLite::WIRETYPE_LENGTH_DELIMITED
            , gpi::WireFormatLite::TYPE_INT32
            , gp::int32
            > (id, item, tag, input);
}
//--------------------------------------------------------------------------
// static inline
// gp::uint32 readVarint (Id id, Id item, Codec::InputStream& input)
// {
//     gp::uint32 varint;
//     bool ok = input.ReadVarint32 (&varint);
//     if (!ok)
//         throw DecodeException (id, item, "readVarint");
//     return varint;
// }
//--------------------------------------------------------------------------
// static inline
// gp::uint32 read32bits (Id id, Id item, Codec::InputStream& input)
// {
//   gp::uint32 value;
//   bool ok = input.ReadLittleEndian32 (&value);
//   if (!ok)
//     throw DecodeException (id, item, "read32bits");
//   return value;
// }


//--------------------------------------------------------------------------
template <typename valueType> inline
void insert (Id msg, Id item, valueType value, ValuesById& values)
{
    // map::insert() returns a 'pair'. When 'pair.second' is 'false' => key was already inserted => throw
    std::pair<ValuesById::iterator,bool> pair =
            values.insert (ValuesById::value_type (item, Variant(value)));
    if (pair.second == false)
        throw DuplicatedFieldIdException (msg, item, Variant::get<valueType>(pair.first->second), value);
}

//--------------------------------------------------------------------------
static inline
const char* position (google::protobuf::io::CodedInputStream& input)
{
    const void* data;
    int         dsize;
    input.GetDirectBufferPointerInline (&data, &dsize);
    const char* ptr = reinterpret_cast<const char*> (data);
    return ptr;
}



//--------------------------------------------------------------------------
void Codec::decodeBool (Message& msg, Id item, gp::uint32 tag, InputStream& input)
{
    bool value = readAny< gpi::WireFormatLite::WIRETYPE_VARINT
            , gpi::WireFormatLite::TYPE_BOOL
            , bool
            > (msg.id(), item, tag, input);

    // map::insert() returns a 'pair'. When 'pair.second' is 'false' => key was already inserted => throw
    std::pair<ValuesById::iterator,bool> pair =
            msg.values_.insert (ValuesById::value_type (item, Variant(value)));
    if (pair.second == false)
        throw DuplicatedFieldIdException (msg.id(), item, pair.first->second.toBool(std::nothrow), value);
}
//--------------------------------------------------------------------------
void Codec::decodeLong (Message& msg, Id item, gp::uint32 tag, InputStream& input)
{
    Variant::long_t value = readLong (msg.id(), item, tag, input);

    // map::insert() returns a 'pair'. When 'pair.second' is 'false' => key was already inserted => throw
    std::pair<ValuesById::iterator,bool> pair =
            msg.values_.insert (ValuesById::value_type (item, Variant(value)));
    if (pair.second == false)
        throw DuplicatedFieldIdException (msg.id(), item, pair.first->second.toLong(std::nothrow), value);
}
//--------------------------------------------------------------------------
void Codec::decodeDouble (Message& msg, Id item, gp::uint32 tag, InputStream& input, Type type)
{
    double value = readAny< gpi::WireFormatLite::WIRETYPE_FIXED64
            , gpi::WireFormatLite::TYPE_DOUBLE
            , double
            > (msg.id(), item, tag, input);

    // map::insert() returns a 'pair'. When 'pair.second' is 'false' => key was already inserted => throw
    std::pair<ValuesById::iterator,bool> pair =
            msg.values_.insert (ValuesById::value_type (item, Variant(type,value)));
    if (pair.second == false)
        throw DuplicatedFieldIdException (msg.id(), item, pair.first->second.toDouble(std::nothrow), value);
}
//--------------------------------------------------------------------------
void Codec::decodeStr (Message& msg, Id item, gp::uint32 tag, InputStream& input)
{
    std::string value;
    readString (msg.id(), item, value, tag, input);

    // map::insert() returns a 'pair'. When 'pair.second' is 'false' => key was already inserted => throw
    std::pair<ValuesById::iterator,bool> pair =
            msg.values_.insert (ValuesById::value_type (item, Variant(value)));
    if (pair.second == false)
        throw DuplicatedFieldIdException (msg.id(), item, pair.first->second.toString(std::nothrow), value);
}

#ifdef USE_INT32_TO_SERIALISE_SUB_MESSAGE
//----------------------------------------
// this is called by decode(input,tag) and decodeSeqMsg(msg,item,tag,input)
// Decode inner message (can be composed of other inner messages)
//----------------------------------------
inline void Codec::decode (Message& msg, InputStream& input, int count)
{
    assert(count > 0);

    PBART_TRACE("decode (msg="<< msg.id() <<", input="<< &input <<", count="<< count <<")\n");

    // stop decode when count reaches zero
    // if count is not zero but tag is zero => there is a problem
    gp::uint32 tag;
    while(  (count--)  &&  (tag = input.ReadTag())  )
    {
        PBART_TRACE ("-loop\n");
        decode (msg, input, tag);
    }

    PBART_TRACE("-end count="<< count <<" tag="<< tag <<" BytesUntilLimit="<< input.BytesUntilLimit() <<'\n');

    // tag is zero => there is a decoding problem...
    if (tag == 0)
        throw DecodeException (msg.id(), count, input.BytesUntilLimit());
}

//----------------------------------------
void Codec::decodeMsg (Message& msg, Id item, gp::uint32 tag, InputStream& input)
{
    int count = readLong (msg.id(), item, tag, input);

    // map::insert() returns a 'pair'. When 'pair.second' is 'false' => key was already inserted => throw
    std::pair<ValuesById::iterator,bool> pair =
            msg.values_.insert (ValuesById::value_type (item, Variant(Message (msg.dico(), item))));
    if (pair.second == false)
        throw DuplicatedFieldIdException (msg.id(), item);

    if (count)
    {
        Message& inner = pair.first->second.toMessage(std::nothrow);
        decode  (inner, input, count);
    }

    assert(msg.get(item).toMessage(std::nothrow).size() == count);
}

#else

//----------------------------------------
// this is called by decode(input,tag) and decodeSeqMsg(msg,item,tag,input) and decodeMsg(msg,item,tag,input,sub)
// Decode inner message (can be composed of other inner messages)
//--------------------------------------------------------------------------
inline void Codec::decode (Message& msg, InputStream& input, int bytes)
{
    assert(bytes > 0);

    // compute end of serialized message within the input proto buffer
    const char* end = position(input) + bytes;

    // decode each element and append it into the array
    gp::uint32 tag = 0;
    while(  position(input) < end  &&  (tag = input.ReadTag())  )
    {
        decode (msg, input, tag);
    }

    // tag is zero => there is a decoding problem...
    if (tag == 0)
        throw DecodeException (msg.id(), end - position(input), input.BytesUntilLimit());

    if (position(input) != end)
        throw DecodeException (msg.id(), "mismatch of sequence size within input protobuf");
}

//--------------------------------------------------------------------------
inline
void Codec::decodeMsg (Message& msg, Id item, gp::uint32 tag, InputStream& input, Message& sub)
{
    // check tag and read message bytes
    int bytes = readLength (msg.id(), item, tag, input);
    if (bytes)
    {
        assert (bytes >= 0);
        decode (sub, input, bytes);
        assert (!bytes == !sub.size());
        sub.bytes_ = bytes;
    }
}

//--------------------------------------------------------------------------
void Codec::decodeMsg (Message& msg, Id item, gp::uint32 tag, InputStream& input)
{
    // map::insert() returns a 'pair'. When 'pair.second' is 'false' => key was already inserted => throw
    std::pair<ValuesById::iterator,bool> pair =
            msg.values_.insert (ValuesById::value_type (item, Variant(Message (msg.dico(), item))));
    if (pair.second == false)
        throw DuplicatedFieldIdException (msg.id(), item);

    Message& sub = pair.first->second.toMessage(std::nothrow);
    decodeMsg (msg, item, tag, input, sub);
}

#endif

//--------------------------------------------------------------------------
void Codec::decodeSeqBool (Message& msg, Id item, int count, InputStream& input )
{
    const gpi::WireFormatLite::FieldType fieldType = gpi::WireFormatLite::TYPE_BOOL;

    // insert empty array
    std::pair<ValuesById::iterator,bool> pair =
            msg.values_.insert (ValuesById::value_type (item, Type(Type::SEQ_BOOL)));
    // map::insert() returns a 'pair'. When 'pair.second' is 'false' => key was already inserted => throw
    if (pair.second == false)
        throw DuplicatedSeqIdException (msg.id(), item);

    // reuse the variant vector in order to fill it
    Variant::SeqBool& vector = pair.first->second.toSeqBool(std::nothrow);

    while (count--)
    {
        bool element;
        bool ok = gpi::WireFormatLite::ReadPrimitive <bool, fieldType> (&input, &element);
        if (!ok)
            throw DecodeException (msg.id(), item, fieldType);

        vector.push_back (element);
    }

    // if (position(input) != end)
    //   throw DecodeException (msg.id(), "mismatch of sequence size within input protobuf", item);
}

//--------------------------------------------------------------------------
void Codec::decodeSeqLong (Message& msg, Id item, int bytes, InputStream& input)
{
    const gpi::WireFormatLite::FieldType fieldType = gpi::WireFormatLite::TYPE_INT64;
    typedef Variant::SeqLong  VectorType;
    typedef Variant::long_t   ValueType;

    // insert empty array
    std::pair<ValuesById::iterator,bool> pair =
            msg.values_.insert (ValuesById::value_type (item, Type(Type::SEQ_LONG)));
    // map::insert() returns a 'pair'. When 'pair.second' is 'false' => key was already inserted => throw
    if (pair.second == false)
        throw DuplicatedSeqIdException (msg.id(), item);

    // reuse the inserted array in order to fill it
    //VectorType& vector = boost::get<VectorType>(*pair.first->second);
    VectorType& vector = pair.first->second.toSeqLong(std::nothrow);

    // compute end of serialized array within the input proto buffer
    const char* end = position(input) + bytes;

    // decode each element and append it into the array
    while (position(input) < end)
    {
        ValueType element;
        bool ok = gpi::WireFormatLite::ReadPrimitive <ValueType, fieldType> (&input, &element);
        if (!ok)
            throw DecodeException (msg.id(), item, fieldType);

        vector.push_back (element);
    }

    if (position(input) != end)
        throw DecodeException (msg.id(), "mismatch of sequence size within input protobuf", item);
}

//--------------------------------------------------------------------------
void Codec::decodeSeqDouble (Message& msg, Id item, int bytes, InputStream& input, Type type)
{
    const gpi::WireFormatLite::FieldType fieldType = gpi::WireFormatLite::TYPE_DOUBLE;

    // insert empty array
    std::pair<ValuesById::iterator,bool> pair =
            msg.values_.insert (ValuesById::value_type (item, type));
    // map::insert() returns a 'pair'. When 'pair.second' is 'false' => key was already inserted => throw
    if (pair.second == false)
        throw DuplicatedSeqIdException (msg.id(), item);

    // reuse the inserted array in order to fill it
    Variant::SeqDouble& vector = pair.first->second.toSeqDouble(std::nothrow);

    // // compute end of serialized array within the input proto buffer
    // const char* end = position(input) + bytes;
    int count = bytes / 8;
    assert (bytes % 8 == 0);

    PBART_TRACE ("decodeSeqDouble (msg="<< msg.id() <<", item="<< item <<", bytes="<< bytes <<", input="<< &input <<") count="<< count <<'\n');

    // // decode each element and append it into the array
    // while (position(input) < end)
    while (count--)
    {
        PBART_TRACE("-loop\n");

        double element;
        bool ok = gpi::WireFormatLite::ReadPrimitive <double, fieldType> (&input, &element);
        if (!ok)
            throw DecodeException (msg.id(), item, fieldType);

        vector.push_back (element);
    }

    PBART_TRACE("-end\n");

    // if (position(input) != end)
    //   throw DecodeException (msg.id(), "mismatch of sequence size within input protobuf", item);
}

//--------------------------------------------------------------------------
#ifdef USE_PACKETED_CODING_FOR_SEQUENCE_OF_LENGTH_DELIMITED_ITEMS
void Codec::decodeSeqStr (Message& msg, Id item, gp::uint32 tag, InputStream& input)
{
    typedef Variant::SeqString VectorType;

    // check tag and read sequence bytes
    int bytes  =  readLength (msg.id(), item, tag, input);

    // compute end of serialized array within the input proto buffer
    const char* end = position(input) + bytes;

    // insert empty array
    std::pair<ValuesById::iterator,bool> pair =
            msg.values_.insert (ValuesById::value_type (item, Type(Type::SEQ_STRING)));
    // map::insert() returns a 'pair'. When 'pair.second' is 'false' => key was already inserted => throw
    if (pair.second == false)
        throw DuplicatedSeqIdException (msg.id(), item);

    // reuse the inserted array in order to fill it
    //VectorType& vector = boost::get<VectorType>(*pair.first->second);
    VectorType& vector = pair.first->second.toSeqString(std::nothrow);

    // decode each element and append it into the array
    size_t vsize = vector.size();
    while (position(input) < end)
    {
        // append an empty element in the array
        size_t index = vsize++;
        vector.resize (vsize);
        VectorType::reference element = vector[index];

        // decode directly into the empty appended element
        bool ok = readString (element, input);
        if (!ok)
            throw DecodeException (msg.id(), item, "string");
    }

    if (position(input) != end)
        throw DecodeException (msg.id(), "mismatch on size of string sequence within input protobuf", item);
}
#else  //USE_PACKETED_CODING_FOR_SEQUENCE_OF_LENGTH_DELIMITED_ITEMS
void Codec::decodeSeqStr (Message& msg, Id item, gp::uint32 /*tag*/, InputStream& input)
{
    Variant::SeqString& vector = msg.get(item).toSeqString();
    vector.push_back(std::string());
    std::string& value = vector.back();
    bool ok = readString (value, input);
    if (!ok)
        throw DecodeException (msg.id(), item, "string");
}
#endif //USE_PACKETED_CODING_FOR_SEQUENCE_OF_LENGTH_DELIMITED_ITEMS

//--------------------------------------------------------------------------
void Codec::decodeSeqMsg (Message& msg, Id item, gp::uint32 tag, InputStream& input)
{
#ifdef USE_PACKETED_CODING_FOR_SEQUENCE_OF_LENGTH_DELIMITED_ITEMS
    typedef Variant::SeqMessage VectorType;

    // check tag and read sequence length
    int bytes  =  readLength (msg.id(), item, tag, input);

    // compute end of serialized array within the input proto buffer
    const char* end = position(input) + bytes;

    // insert empty array
    std::pair<ValuesById::iterator,bool> pair =
            msg.values_.insert (ValuesById::value_type (item, Type(Type::SEQ_MESSAGE)));
    // map::insert() returns a 'pair'. When 'pair.second' is 'false' => key was already inserted => throw
    if (pair.second == false)
        throw DuplicatedSeqIdException (msg.id(), item);

    // reuse the inserted array in order to fill it
    //VectorType& vector = boost::get<VectorType>(*pair.first->second);
    VectorType& vector = pair.first->second.toSeqMessage(std::nothrow);

    assert(msg.get(item).toSeqMessage(std::nothrow).size() == 0);

    // decode each element and append it into the array
    while (position(input) < end)
    {
        int count = readVarint (msg.id(), item, input);
        vector.push_back (Message(msg.dico(), item));
        if (count)
        {
            decode (vector.back(), input, count);
#ifdef USE_INT32_TO_SERIALISE_SUB_MESSAGE
        }
        assert(msg.get(item).toSeqMessage(std::nothrow)[msg.get(item).toSeqMessage(std::nothrow).size() - 1].size() == count);
#else
            vector.back().bytes_ = count;
            assert( vector.back().bytes_ >= 0 );
            assert(!vector.back().bytes_ == !vector.back().size());
        }
#endif
    }

    if (position(input) != end)
        throw DecodeException (msg.id(), "mismatch on size of Message sequence within input protobuf", item);
#else  //USE_PACKETED_CODING_FOR_SEQUENCE_OF_LENGTH_DELIMITED_ITEMS
    Variant::SeqMessage& vector = msg.get(item).toSeqMessage();
    vector.push_back (Message (msg.dico(), item));
    Message& sub = vector.back();
    decodeMsg (msg, item, tag, input, sub);
#endif //USE_PACKETED_CODING_FOR_SEQUENCE_OF_LENGTH_DELIMITED_ITEMS
}

//--------------------------------------------------------------------------
void Codec::decodeSeqBool (Message& msg, Id item, gp::uint32 tag, InputStream& input)
{
    // check tag and read sequence length
    int bytes  =  readLength (msg.id(), item, tag, input);

#ifdef USE_BOOST_DYNAMIC_BITSET
    if (bytes == 1 + 4)  // 1 byte + 4 bytes
    {
        gp::uint32 nb  =  readVarint (msg.id(), item, input); // 1 byte  => the number of relevant bits
        if (nb<4 || nb>32)
            throw DecodeException (msg.id(), item, "Number of bits should be in range [4..32] retrieved value is nb=", nb);

        gp::uint32 bits = read32bits (msg.id(), item, input); // 4 bytes => the value of the bits

        // insert array
        std::pair<ValuesById::iterator,bool> pair =
                msg.values_.insert (ValuesById::value_type (item, Variant(Variant::SeqBool(nb,bits))));
        // map::insert() returns a 'pair'. When 'pair.second' is 'false' => key was already inserted => throw
        if (pair.second == false)
            throw DuplicatedSeqIdException (msg.id(), item);
    }
    else
#endif
    {
        // read element and insert them one by one
        decodeSeqBool (msg, item, bytes, input);
    }
}

//--------------------------------------------------------------------------
void Codec::decodeSeqLong (Message& msg, Id item, gp::uint32 tag, InputStream& input)
{
    // check tag and read sequence length
    int bytes  =  readLength (msg.id(), item, tag, input);

    // read element and insert them one by one
    decodeSeqLong (msg, item, bytes, input);
}

//--------------------------------------------------------------------------
void Codec::decodeSeqDouble (Message& msg, Id item, gp::uint32 tag, InputStream& input, Type type)
{
    // check tag and read sequence length
    int bytes  =  readLength (msg.id(), item, tag, input);

    // read element and insert them one by one
    decodeSeqDouble (msg, item, bytes, input, type);
}

//--------------------------------------------------------------------------
inline void Codec::decode (Message& msg, InputStream& input, gp::uint32 tag)
{
    assert( gpi::WireFormatLite::GetTagFieldNumber(tag) >= ID_MIN );
    assert( gpi::WireFormatLite::GetTagFieldNumber(tag) <= ID_MAX );
    Id item = static_cast<Id>( gpi::WireFormatLite::GetTagFieldNumber(tag) );

    // check if item ID is correct
    // if same ID => check header and return
    assert(msg.def_); //assert(msg.def_->type() == Type::MESSAGE);
    ItemsById::const_iterator it = msg.def_->items().find(item);
    if (it == msg.def_->items().end())
    {
        if (msg.id() != item)
        {
            throw ItemException (msg.id(), item); //this item is not part of the message
        }

        std::string data = header (input, tag, item);
        assert(msg.dico());
        if (data != msg.dico()->header())
        {
            if (msg.empty() && msg.id()==Serializer::dicoId_ && data==Serializer::dicoOfDico()->header())
            {
                msg = Message (Serializer::dicoOfDico(), Serializer::dicoId_);
            }
            else
            {
                throw DecodeException (msg.id(), msg.dico()->header(), data); //TODO(Oliver): try to reload again dico (XML file)
            }
        }
        return;
    }

#ifdef USE_DIRECT_POINTER_ACCESS
    assert(msg.dico());
    assert(msg.dico()->field(it->first).type() == it->second.ptr()->type());
#endif

    const Type type = msg.type(it->first);
    switch (type)
    {
    case Type::EMPTY:
        assert(0&&"Unexpected Type::EMPTY from msg.type(it->first)");
        BOOST_FALLTHROUGH; // for optimization continue with next case
    default:
        assert(0&&"Unexpected type from msg.type(it->first)");
        BOOST_FALLTHROUGH; // for optimization continue with next case

    case Type::BOOL:
        decodeBool (msg, item, tag, input);
        break;

    case Type::LONG:
        decodeLong (msg, item, tag, input);
        break;

    case Type::DOUBLE:
        decodeDouble (msg, item, tag, input, type);
        break;

    case Type::STRING:
        decodeStr (msg, item, tag, input);
        break;

    case Type::MESSAGE:
        decodeMsg (msg, item, tag, input);
        break;

    case Type::SEQ_BOOL:
        decodeSeqBool (msg, item, tag, input);
        break;

    case Type::SEQ_LONG:
        decodeSeqLong (msg, item, tag, input);
        break;

    case Type::SEQ_DOUBLE:
        decodeSeqDouble (msg, item, tag, input, type);
        break;

    case Type::SEQ_STRING:
        decodeSeqStr (msg, item, tag, input);
        break;

    case Type::SEQ_MESSAGE:
        decodeSeqMsg (msg, item, tag, input);
        break;
    }
}

//--------------------------------------------------------------------------
// this is called by public Codec::decode()
// Decode global message (can be composed of inner messages)
void Codec::decode (Message& msg, InputStream& input)
{
    if (msg.id() == 0)
    {
        gp::uint32 tag = input.ReadTag();
        Id id = gpi::WireFormatLite::GetTagFieldNumber(tag);
        std::string data = header (input, tag, msg.id());
        if (msg.dico() && msg.dico()->header() == data)
        {
            msg.id(id);
        }
        else if (id == Serializer::dicoId_ && data == Serializer::dicoOfDico()->header())
        {
            msg = Message (Serializer::dicoOfDico(), Serializer::dicoId_);
        }
        else
        {
            throw DecodeException (id, msg.dico()->header(), data); //TODO(Oliver): try to reload again dico (XML file)
        }
    }

    //  tag = zero => means the End of InputStream
    while( gp::uint32 tag = input.ReadTag() )
    {
        decode (msg, input, tag);
    }
}

} // namespace pbart
