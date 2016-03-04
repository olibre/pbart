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
#include "pbart/Exception.h"
#include "pbart/Message.h"

#include <string>


namespace pbart
{

//--------------------------------------------------------------------------
// Type
//--------------------------------------------------------------------------

std::string ExceptionHelper::type (Id msg, Id item, Type type_from_variant, Type type_from_dico)
{
    std::ostringstream oss;
    oss <<"In message id="<< msg <<" the item id="<< item
       <<" use wrong type='"<< type_from_variant.str()
      <<"' (must be type="<<  type_from_dico   .str() <<')';
    return oss.str();
}

std::string ExceptionHelper::type (Type current, Type requested)
{
    std::string str;
    str += "Attempting to access variant as a '";
    str += requested.str();
    if (current == Type::EMPTY)
    {
        str += "' but the variant is EMPTY (blank)";
    }
    else
    {
        str += "' but the variant contains a '";
        str += current.str();
        str += '\'';
    }
    return str;
}

template <>
std::string ExceptionHelper::type<Message> (const Message&, Id)
{
    return "";
}

//--------------------------------------------------------------------------
// Id
//--------------------------------------------------------------------------

// bug in MS-VisualC++ 2010:
// badid<Field>() is not used for IdException(const Field&,Id)
// within Item const& Field::item (Id id) const
// => comment below lines and us generic version in Exception.h
#ifndef _MSC_VER

template<>
std::string ExceptionHelper::badid<Message> (const Message& msg, Id item)
{
    std::ostringstream oss;
    if (msg.name().empty())
    {
        oss <<"Use an invalid message id="<< msg.id() <<" name=''";
    }
    else
    {
        oss <<"Use message "<< msg.name();
    }
    oss <<" and attempt to access item id="<< item <<" but this item is not part of the dictionary";
    return oss.str();
}

template<>
std::string ExceptionHelper::badid<Field> (const Field& msg, Id item)
{
    std::ostringstream oss;
    if (msg.name().empty() || msg.type()!=Type::MESSAGE)
    {
        oss <<"Use an invalid message name='"<< msg.name() <<"' type="<< msg.type().str();
    }
    else
    {
        oss <<"Use message "<< msg.name();
    }
    oss <<" and attempt to access item id="<< item <<" but this item is not part of the dictionary";
    return oss.str();
}

#endif //_MSC_VER

//--------------------------------------------------------------------------
// Empty
//--------------------------------------------------------------------------

std::string ExceptionHelper::empty (Id id)
{
    std::ostringstream oss;
    oss <<"Item ID="<< id <<" is not present in the map and therefore cannot be accessed => before use Message::set(id,val)";
    return oss.str();
}


//--------------------------------------------------------------------------
// Out of range
//--------------------------------------------------------------------------

std::string ExceptionHelper::out (Type type, size_t index, size_t length)
{
    std::ostringstream oss;
    oss <<"Attempting to access a variant array '"<< type.str() <<"' but index="<< index <<" is greater than or equal array length="<< length;
    return oss.str();
}


//--------------------------------------------------------------------------
// Decode
//--------------------------------------------------------------------------

std::string ExceptionHelper::decode (Id id, const char reason[])
{
    std::ostringstream oss;
    oss <<"Error while decoding message id="<< id <<" because "<< reason;
    return oss.str();
}

std::string ExceptionHelper::decode (Id id, const char reason[], Id item)
{
    std::ostringstream oss;
    oss <<"Error while decoding message id="<< id <<" at item id="<< item <<" because "<< reason;
    return oss.str();
}

//--------------------------------------------------------------------------

std::string ExceptionHelper::decode (Id requestedId, Id idFromBuffer)
{
    assert (requestedId > ID_MIN);
    assert (requestedId < ID_MAX);

    std::ostringstream oss;
    oss <<"Error while decoding message ";
    //if (requestedId)
    {
        oss <<" (requested id="<< requestedId <<'\'';
    }

    //if (idFromBuffer)
    {
        oss <<" because buffer contains a different id="<< idFromBuffer;
    }
    //else
    //{
    //  oss <<" because buffer is empty";
    //}

    return oss.str();
}

//--------------------------------------------------------------------------

static inline std::string decodeError (Id id, const std::string& dicoHeader, const std::string& msgHeader)
{
    std::ostringstream oss;
    if (dicoHeader.size() == msgHeader.size())
    {
        oss <<"Header mismatch while decoding message id="<< id;
    }
    else
    {
        oss <<"Cannot reconize header while decoding message";
    }
    oss <<". Expected header from Dico: size="<< dicoHeader.size() <<" content=";
    for (std::string::const_iterator it = dicoHeader.begin(); it != dicoHeader.end(); oss <<'['<< int(*it++) <<']');
    oss <<". First LENGTH_DELIMITED retrieved from message: size="<< msgHeader.size() <<" content=";
    for (std::string::const_iterator it = dicoHeader.begin(); it != dicoHeader.end(); oss <<'['<< int(*it++) <<']');
    oss <<". Header format must be [Version][Checksum1][Checksum2]";
    return oss.str();
}

DecodeException::DecodeException (Id id, const std::string& dicoHeader, const std::string& msgHeader)
    : CodecException(decodeError(id,dicoHeader,msgHeader)) { }

//--------------------------------------------------------------------------

static inline std::string decodeError (Id msg, Id field, const char type[])
{
    std::ostringstream oss;
    oss <<"Error while decoding message id="<< msg
       <<" because the item id="<< field
      <<" cannot be decoded as protobuf type="<< type;
    return oss.str();
}

DecodeException::DecodeException (Id msg, Id field, const char type[])
    : CodecException(decodeError (msg, field, type))   { }

//--------------------------------------------------------------------------

static inline std::string decodeError (Id msg, Id field, const char type[], int val)
{
    std::ostringstream oss;
    oss <<"Error while decoding message id="<< msg
       <<" and item id="<< field
      <<" because "<< type << val;
    return oss.str();
}

DecodeException::DecodeException (Id msg, Id field, const char type[], int val)
    : CodecException(decodeError (msg, field, type, val))   { }

//--------------------------------------------------------------------------

static inline std::string decodeError (Id msg, int count, int bytesUntilLimit)
{
    std::ostringstream oss;
    oss <<"Error while decoding message id="<< msg
       <<" because when count="<< count <<" is not zero"
                                          " the bytesUntilLimit="<< bytesUntilLimit <<" must be zero";
    return oss.str();
}

DecodeException::DecodeException (Id msg, int count, int bytesUntilLimit)
    : CodecException(decodeError (msg, count, bytesUntilLimit))  { }

//--------------------------------------------------------------------------
// Item
//--------------------------------------------------------------------------

static inline std::string unexpectedId (Id msg, Id unexpected)
{
    std::ostringstream oss;
    oss <<"Error while decoding message id="<< msg
       <<" found item id="<< unexpected
      <<" but this item is not part of the message";
    return oss.str();
}

ItemException::ItemException (Id m, Id u)
    : DecodeException(unexpectedId(m,u))  { }


//--------------------------------------------------------------------------

static inline std::string badMsgType (Id main, int wtype, int dtype)
{
    std::ostringstream oss;
    oss <<"Error while decoding message id="<< main
       <<" because the header has an inconsistent WireFormat type="<< wtype
      <<" must be type="<< dtype;
    return oss.str();
}

MsgTypeException::MsgTypeException (Id main, int wtype, int dtype)
    : CodecException (badMsgType (main, wtype, dtype))  { }

//--------------------------------------------------------------------------

static inline std::string badMsgType (Id main, Id inner, int wtype, int dtype)
{
    std::ostringstream oss;
    oss <<"Error while decoding message id="<< main
       <<" because the inner message id="<< inner
      <<" has an inconsistent WireFormat type="<< wtype
     <<" must be type="<< dtype;
    return oss.str();
}

MsgTypeException::MsgTypeException (Id main, Id inner, int wtype, int dtype)
    : CodecException (badMsgType (main, inner, wtype, dtype))  { }

//--------------------------------------------------------------------------

static inline std::string duplicatedFieldId (Id msg, Id field)
{
    std::ostringstream oss;
    oss <<"Error while decoding message id="<< msg
       <<" because the inner message id="<< field <<" is duplicated";
    return oss.str();
}

DuplicatedFieldIdException::DuplicatedFieldIdException (Id msg, Id field)
    : DecodeException (duplicatedFieldId (msg, field))  { }

//--------------------------------------------------------------------------

static inline std::string duplicatedSeqId (Id msg, Id item)
{
    std::ostringstream oss;
    oss <<"Error while decoding message id="<< msg
       <<" because the repeated id="<< item <<" is duplicated";
    return oss.str();
}

DuplicatedSeqIdException::DuplicatedSeqIdException (Id msg, Id field)
    : DecodeException (duplicatedSeqId (msg, field))  { }

} // namespace pbart
