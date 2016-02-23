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

#ifndef PBART_EXCEPTION_H_
#define PBART_EXCEPTION_H_

#include <string>
#include <sstream>
#include <typeinfo>

#include "pbart/Export.h"
#include "pbart/Types.h"
#include "pbart/CommonException.h"

namespace pbart
{

  //--------------------------------------------------------------------------
  // Compute strings for all local Exceptions --> what()
  //--------------------------------------------------------------------------
  // These functions cannot be part of corresponding Exception classes
  // because of the DLL_EXPORT bug in Micosoft/VisualC++:
  //  If a body is defined in a *.cpp
  //  then the class must be declare using PBART_EXPORT
  //  all all the classes it derives must also be PBART_EXPORT
  //  As pbart::Exception derives from std::logic_error
  //  and this class is not declared using PBART_EXPORT
  //  therefore pbart::Exception ans its derivated classes cannot
  //  define body in *.cpp  :(
  //--------------------------------------------------------------------------
  class PBART_EXPORT ExceptionHelper
  {
  public:
    template <typename MSG>
    static std::string badid(const MSG& msg, Id item);
    template <typename MSG>
    static std::string type (const MSG& msg, Id item);
    static std::string type (Id msg, Id item, Type type_from_variant, Type type_from_dico);
    static std::string type (Type current, Type requested);
    static std::string type (Type requested);
//  static std::string dico (const std::string& file, const std::string& reason);
    static std::string empty(Id id);
    static std::string decode (Id id, const char reason[]);
    static std::string decode (Id id, const char reason[], Id item);
    static std::string decode (Id a, Id b);
    static std::string out (Type type, size_t index, size_t length);
  };


  //--------------------------------------------------------------------------
  // PBART Exception
  class Exception : public ::common::Exception
  {
  public:
    explicit Exception(const std::string& what) : ::common::Exception(what) { }
  };


  //--------------------------------------------------------------------------
  // Dico
  class DicoException : public Exception
  {
  public:
    explicit DicoException (const std::string& reason);
    DicoException (const std::string& file, const std::string& reason);
  };


  //--------------------------------------------------------------------------
  // Type
  class TypeException : public Exception
  {
  public:
    TypeException (Id msg, Id item, Type, Type);
    TypeException (Type current, Type requested);
  };


  //--------------------------------------------------------------------------
  // Id
  class IdException : public Exception
  {
  public:
    template <typename MSG>
    IdException (const MSG& msg, Id item);
  };


  //--------------------------------------------------------------------------
  // Empty
  //--------------------------------------------------------------------------
  // used when mutable iterator has to return a reference to an empty value
  // in all the other cases a default value is returned :
  // - iterator const get returns a const reference the default value (variant) from dico
  // - message  const get does the same
  // - message mutable get inserts the default value (variant) into the values container (and returns its reference)
  // mutable iterator cannot do like mutable message because iterator cannot insert into the values container
  // if mutable iterator is change to perform default value insertion, therefore this exception can be removed
  class EmptyException : public Exception
  {
  public:
    explicit EmptyException (Id id);
  };


  //--------------------------------------------------------------------------
  // Out of range
  class OutOfRangeException : public Exception
  {
  public:
    OutOfRangeException (Type type, size_t index, size_t length);
  };


  //--------------------------------------------------------------------------
  // Codec
  class CodecException : public Exception
  {
  public:
    explicit CodecException (const std::string& str);
    explicit CodecException (const char reason[]);
  };

  //--------------------------------------------------------------------------
  class MsgTypeException : public CodecException
  {
  public:
    template <typename MSG>
    MsgTypeException (const MSG& msg, Id item);
    MsgTypeException (Id main, int wtype, int dtype);
    MsgTypeException (Id main, Id inner, int wtype, int dtype);
  };

  //--------------------------------------------------------------------------
  // Decode
  class DecodeException : public CodecException
  {
  public:
    explicit DecodeException (const std::string& str);
    explicit DecodeException (const char reason[]);
    DecodeException (Id id, const char reason[]);
    DecodeException (Id id, const char reason[], Id item);
    DecodeException (Id a, Id b);
    DecodeException (Id id, const std::string& dicoHeader, const std::string& msgHeader);
    DecodeException (Id msg, Id field, const char type[]);
    DecodeException (Id msg, Id field, const char type[], int val);
    DecodeException (Id msg, int count, int bytesUntilLimit);
  };

  //--------------------------------------------------------------------------
  class ItemException : public DecodeException
  {
  public:
    ItemException (Id msg, Id unexpected);
  };

  //--------------------------------------------------------------------------
  class DuplicatedFieldIdException : public DecodeException
  {
  public:
    template <typename T>
    DuplicatedFieldIdException (Id msg, Id field, T inval, T outval);
    DuplicatedFieldIdException (Id msg, Id field);
  };

  //--------------------------------------------------------------------------
  class DuplicatedSeqIdException : public DecodeException
  {
  public:
    DuplicatedSeqIdException (Id msg, Id item);
  };

} // namespace pbart


// INLINE


namespace pbart
{
  //--------------------------------------------------------------------------
  // Dico
  //--------------------------------------------------------------------------

  inline DicoException::DicoException (const std::string& r)
    : Exception(r)  { }

//  inline DicoException::DicoException (const std::string& f, const std::string& r)
//    : Exception( ExceptionHelper::dico(f,r))  { }


  //--------------------------------------------------------------------------
  // Type
  //--------------------------------------------------------------------------

  inline TypeException::TypeException (Id msg, Id item, Type type_from_variant, Type type_from_dico)
    : Exception (ExceptionHelper::type (msg, item, type_from_variant, type_from_dico))  { }

  inline TypeException::TypeException (Type current, Type requested)
    : Exception (ExceptionHelper::type (current, requested))  { }

  template <typename MSG>
  MsgTypeException::MsgTypeException (const MSG& msg, Id item)
    : CodecException (ExceptionHelper::type (msg, item))  { }


  //--------------------------------------------------------------------------
  // Empty
  //--------------------------------------------------------------------------

  inline EmptyException::EmptyException(Id id)
    : Exception( ExceptionHelper::empty(id) )   { }


  //--------------------------------------------------------------------------
  // Id
  //--------------------------------------------------------------------------

  // bug in MS-VisualC++ 2010:
  // Below template function to be defined
  // because the specialized version badid<Field>()
  // defined in Exception.cpp is not used for
  // IdException(const Field&,Id)
  // within Item const& Field::item (Id id) const
#ifdef _MSC_VER
  template <typename FIELD>
  std::string ExceptionHelper::badid (const FIELD& field, Id item)
  {
    std::ostringstream oss;
    if (field.name().empty() || field.type()!=Type::MESSAGE)
    {
      oss <<"Use an invalid message name='"<< field.name() <<"' type="<< field.type().str();
    }
    else
    {
      oss <<"Use message "<< field.name();
    }
    oss <<" and attempt to access item id="<< item <<" but this item is not part of the dictionary";
    return oss.str();
  }
#endif

  template <typename MSG>
  IdException::IdException (const MSG& msg, Id item)
    : Exception( ExceptionHelper::badid(msg,item) )  { }


  //--------------------------------------------------------------------------
  // Out of range
  //--------------------------------------------------------------------------

  inline OutOfRangeException::OutOfRangeException (Type type, size_t index, size_t length)
    : Exception( ExceptionHelper::out (type, index, length) )   { }


  //--------------------------------------------------------------------------
  // Codec
  //--------------------------------------------------------------------------

  inline CodecException::CodecException (const std::string& str)
    : Exception(str) { }

  inline CodecException::CodecException (const char r[])
    : Exception(r)   { }


  //--------------------------------------------------------------------------
  // Decode
  //--------------------------------------------------------------------------

  inline DecodeException::DecodeException (const std::string& str)
    : CodecException(str) { }

  inline DecodeException::DecodeException (const char r[])
    : CodecException(r)   { }

  //--------------------------------------------------------------------------

  inline DecodeException::DecodeException (Id id, const char r[])
    : CodecException( ExceptionHelper::decode(id,r))      { }

  inline DecodeException::DecodeException (Id id, const char r[], Id item)
    : CodecException( ExceptionHelper::decode(id,r,item)) { }

  //--------------------------------------------------------------------------

  inline DecodeException::DecodeException (Id a, Id b)
    : CodecException(ExceptionHelper::decode(a,b))    { }

  //--------------------------------------------------------------------------

  template <typename T>
  std::string duplicatedFieldId (Id msg, Id field, T inval, T outval)
  {
    std::ostringstream oss;
    oss <<"Error while decoding message id="<< msg
        <<" because the field id="<< field <<" is duplicated:"
          " value already inserted="<< inval <<
          " value not inserted="<<    outval;
    return oss.str();
  }

  template <typename T>
  DuplicatedFieldIdException::DuplicatedFieldIdException (Id msg, Id field, T inval, T outval)
    : DecodeException (duplicatedFieldId (msg, field, inval, outval))  { }

} // namespace pbart


#endif  // PBART_EXCEPTION_H_

