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

#ifndef PBART_TYPES_H_
#define PBART_TYPES_H_

#include <boost/cstdint.hpp> //uint_fast8_t for C++03/C++11 Windows/Linux/...
#include <climits>
#include <string>
#include <cassert>
#include <vector>

#include "pbart/Export.h"

namespace pbart
{
// -----------------------------------------------------------
// ID of Fields and Messages (unique identification number)
// -----------------------------------------------------------
// 'short' is specified in C++ standard to be at least 16 bits wide
// (therefore 'short' is variable-width dependent on implementation)
// 'int16_t' is specified to be fixed-exact-width => use INT16_MAX / INT16_MIN
// However 'int_fast16_t' should be more CPU-Efficient...
// Please refer to http://en.wikipedia.org/wiki/Stdint.h#stdint.h
typedef int_fast16_t /*int16_t*/ Id;         // Id cannot be
static  const int  ID_MAX  =  SHRT_MAX;      // - greater than ID_MAX
static  const int  ID_MIN  =  0; //excluded  // - less than or equal ID_MIN

// -----------------------------------------------------------
// native type of IDL sequence<octet>
typedef std::string Buffer;

//-----------------------------------------------------------
// type list and enum must be consistent
class PBART_EXPORTS Type
{

public:
    typedef uint_fast8_t /*int8_t*/ Integer;

#if __cplusplus < 201103L
    static const Integer EMPTY       =  0;//boost::blank
    static const Integer BOOL        =  1;// order is important
    static const Integer LONG        =  2;// must be same as boost::variant<x,y,z> declaration
    static const Integer DOUBLE      =  3;
    static const Integer STRING      =  4;
    static const Integer MESSAGE     =  5;
    static const Integer SEQ_BOOL    =  6;
    static const Integer SEQ_LONG    =  7;
    static const Integer SEQ_DOUBLE  =  8;
    static const Integer SEQ_STRING  =  9;
    static const Integer SEQ_MESSAGE = 10;
    typedef Integer Enum;
#else
    enum Enum : Integer
    {
        EMPTY       =  0,//boost::blank
        BOOL        =  1,// order is important
        LONG        =  2,// must be same as boost::variant<x,y,z> declaration
        DOUBLE      =  3,
        STRING      =  4,
        MESSAGE     =  5,
        SEQ_BOOL    =  6,
        SEQ_LONG    =  7,
        SEQ_DOUBLE  =  8,
        SEQ_STRING  =  9,
        SEQ_MESSAGE = 10
    };
#endif
    static const Integer SEQ_DELTA   = SEQ_BOOL - BOOL;

public:
    Type ()                              : type_( EMPTY )        { }
    Type (Enum    v)                     : type_(     v )        { }
    Type (Integer v)                     : type_(Enum(v))        { }
    explicit Type (const std::string& s) : type_( parse(s) )     { }
    explicit Type (const char*        s) : type_( parse(s) )     { }
    bool    repeated()  const            { return type_ > MESSAGE; }
    bool   primitive()  const            { return type_ < MESSAGE; }
    bool      scalar()  const            { return basic()< STRING; }
    operator    Enum()  const            { return type_;           }
    Enum        type()  const            { return type_;           }

    void setSequence()
    {
        assert( type_ >= BOOL     );
        assert( type_ <= SEQ_BOOL );

        Integer tmp = type_;
        tmp += SEQ_DELTA;
        type_ = Enum(tmp);
    }

    Enum basic() const
    {
        if( repeated() )
        {
            assert( type_ >= SEQ_BOOL    );
            assert( type_ <= SEQ_MESSAGE );

            Integer tmp = type_;
            tmp -= SEQ_DELTA;
            return Enum(tmp);
        }
        else
        {
            return type_;
        }
    }


    //-----------------------------------------------------------
    static Enum parse (const std::string& str);

    //-----------------------------------------------------------
    bool check() const { return  type_ <= SEQ_MESSAGE; }
    bool empty() const { return  type_ == EMPTY;       }
    void reset()       {         type_ =  EMPTY;       }


    //#ifdef _MSC_VER
#    define INIT(x)       #x
    //#else
    //#  define INIT(x) [x] = #x
    //#endif

#define ALL_BASIC_TYPES "BOOL, LONG, DOUBLE, STRING"

    //-----------------------------------------------------------
    const char* str() const
    {
        static const char* const tab[SEQ_MESSAGE+1] =
        {
            INIT(EMPTY),
            INIT(BOOL),
            INIT(LONG),
            INIT(DOUBLE),
            INIT(STRING),
            INIT(MESSAGE),
            INIT(SEQ_BOOL),
            INIT(SEQ_LONG),
            INIT(SEQ_DOUBLE),
            INIT(SEQ_STRING),
            INIT(SEQ_MESSAGE)
        };

        assert (check());
        return tab[type_];
    }

private: // DATA
    Enum type_;
};

// helper function for padding leading spaces to generic output stream
template <typename T>
inline void spaces (T& oss, int n)
{
    while (n--) oss.put(' ');
}

} // namespace pbart


//-------------------------------------------------------------------------
// Generic stream operator <<
template <typename T, typename MSG, typename VIt>
T& operator << (T& os, const pbart::Type& type)
{
    return os << type.str() <<'('<< int(type) <<')';
}


#endif  // PBART_TYPES_H_
