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

#ifndef PBART_VARIANT_H_
#define PBART_VARIANT_H_


//Disable bitset optimization to be more compliant with ProtoBuf
//#define USE_BOOST_DYNAMIC_BITSET


#include <string>
#include <vector>

#ifdef USE_BOOST_DYNAMIC_BITSET
#include "boost/dynamic_bitset.hpp"
#endif

#include "pbart/Types.h"
#include "pbart/Exception.h"



namespace pbart
{
// -----------------------------------------------------------
// variant type
template <typename MSG>
class VariantT
{
public:

    // -----------------------------------------------------------
    // 'long' is specified in C++ standard to be at least 32 bits wide
    // (therefore 'long' is variable-width dependent on implementation)
    // 'int32_t' is specified to be fixed-exact-width
    // http://en.wikipedia.org/wiki/Comparison_of_programming_languages_%28basic_instructions%29#endnote_CInt
    typedef int64_t long_t;

#  ifdef USE_BOOST_DYNAMIC_BITSET
    typedef boost::dynamic_bitset<>  SeqBool;
#  else
    typedef std::vector<bool>        SeqBool;
#  endif

    typedef std::vector<long_t>      SeqLong;
    typedef std::vector<double>      SeqDouble;
    typedef std::vector<std::string> SeqString;
    typedef std::vector<MSG>         SeqMessage;

    // Constructors
public:
    VariantT ()                     :           type_(Type::EMPTY)         {}
    VariantT (Type::Integer t     ) :           type_(t)                   {}
    VariantT (Type t              ) :           type_(t)                   {}
    VariantT (Type t, double     v) : u_   (v), type_(t)                   {}
    VariantT (        bool       v) : u_   (v), type_(Type::BOOL)          {}
    VariantT (        int        v) : u_   (v), type_(Type::LONG)          {}
    VariantT (        long       v) : u_   (v), type_(Type::LONG)          {}
    VariantT (        long long  v) : u_   (v), type_(Type::LONG)          {}
    VariantT (        double     v) : u_   (v), type_(Type::DOUBLE)        {}
    VariantT (const std::string& v) : strg_(v), type_(Type::STRING)        {}
    VariantT (const char       * v) : strg_(v), type_(Type::STRING)        {}
    VariantT (const MSG        & v) : msge_(v), type_(Type::MESSAGE)       {}
    VariantT (const SeqBool    & v) : seqB_(v), type_(Type::SEQ_BOOL)      {}
    VariantT (const SeqLong    & v) : seqL_(v), type_(Type::SEQ_LONG)      {}
    VariantT (const SeqDouble  & v) : seqD_(v), type_(Type::SEQ_DOUBLE)    {}
    VariantT (const SeqString  & v) : seqS_(v), type_(Type::SEQ_STRING)    {}
    VariantT (const SeqMessage & v) : seqM_(v), type_(Type::SEQ_MESSAGE)   {}

    // Data
protected:

    union variantType
    {
        variantType() {}
        explicit variantType (bool      v) : bool_(v) {}
        explicit variantType (int       v) : long_(v) {}
        explicit variantType (long      v) : long_(v) {}
        explicit variantType (long long v) : long_(v) {}
        explicit variantType (double    v) : dble_(v) {}

        bool       bool_;
        long_t     long_;
        double     dble_;
    };

    variantType  u_;
    std::string  strg_;
    MSG          msge_;
    SeqBool      seqB_;
    SeqLong      seqL_;
    SeqDouble    seqD_;
    SeqString    seqS_;
    SeqMessage   seqM_;
    Type         type_;

    // Functions
public:
    //---------------------------------------------------------------------------
    bool               toBool      (const std::nothrow_t&) const { assert(type()==Type::BOOL);        return      u_.bool_; }
    long_t             toLong      (const std::nothrow_t&) const { assert(type()==Type::LONG);        return      u_.long_; }
    double             toDouble    (const std::nothrow_t&) const { assert(type()==Type::DOUBLE);      return      u_.dble_; }
    //---------------------------------------------------------------------------
    std::string const& toString    (const std::nothrow_t&) const { assert(type()==Type::STRING);      return         strg_; }
    std::string&       toString    (const std::nothrow_t&)       { assert(type()==Type::STRING);      return         strg_; }
    //---------------------------------------------------------------------------
    MSG         const& toMessage   (const std::nothrow_t&) const { assert(type()==Type::MESSAGE);     return         msge_; }
    MSG&               toMessage   (const std::nothrow_t&)       { assert(type()==Type::MESSAGE);     return         msge_; }
    //---------------------------------------------------------------------------
    SeqBool     const& toSeqBool   (const std::nothrow_t&) const { assert(type()==Type::SEQ_BOOL);    return         seqB_; }
    SeqBool          & toSeqBool   (const std::nothrow_t&)       { assert(type()==Type::SEQ_BOOL);    return         seqB_; }
    //---------------------------------------------------------------------------
    SeqLong     const& toSeqLong   (const std::nothrow_t&) const { assert(type()==Type::SEQ_LONG);    return         seqL_; }
    SeqLong          & toSeqLong   (const std::nothrow_t&)       { assert(type()==Type::SEQ_LONG);    return         seqL_; }
    //---------------------------------------------------------------------------
    SeqDouble   const& toSeqDouble (const std::nothrow_t&) const { assert(type()==Type::SEQ_DOUBLE);  return         seqD_; }
    SeqDouble        & toSeqDouble (const std::nothrow_t&)       {                                    return         seqD_; }
    //---------------------------------------------------------------------------
    SeqString   const& toSeqString (const std::nothrow_t&) const { assert(type()==Type::SEQ_STRING);  return         seqS_; }
    SeqString        & toSeqString (const std::nothrow_t&)       { assert(type()==Type::SEQ_STRING);  return         seqS_; }
    //---------------------------------------------------------------------------
    SeqMessage  const& toSeqMessage(const std::nothrow_t&) const { assert(type()==Type::SEQ_MESSAGE); return         seqM_; }
    SeqMessage       & toSeqMessage(const std::nothrow_t&)       { assert(type()==Type::SEQ_MESSAGE); return         seqM_; }


    //---------------------------------------------------------------------------
    bool               toBool      () const { check (Type::BOOL);        return      u_.bool_; }
    long_t             toLong      () const { check (Type::LONG);        return      u_.long_; }
    double             toDouble    () const { check (Type::DOUBLE);      return      u_.dble_; }
    //--------------------------------------------------------------------
    std::string const& toString    () const { check (Type::STRING);      return         strg_; }
    std::string&       toString    ()       { check (Type::STRING);      return         strg_; }
    //--------------------------------------------------------------------
    MSG         const& toMessage   () const { check (Type::MESSAGE);     return         msge_; }
    MSG&               toMessage   ()       { check (Type::MESSAGE);     return         msge_; }
    //--------------------------------------------------------------------
    SeqBool     const& toSeqBool   () const { check (Type::SEQ_BOOL);    return         seqB_; }
    SeqBool          & toSeqBool   ()       { check (Type::SEQ_BOOL);    return         seqB_; }
    //--------------------------------------------------------------------
    SeqLong     const& toSeqLong   () const { check (Type::SEQ_LONG);    return         seqL_; }
    SeqLong          & toSeqLong   ()       { check (Type::SEQ_LONG);    return         seqL_; }
    //--------------------------------------------------------------------
    SeqDouble   const& toSeqDouble () const { check (Type::SEQ_DOUBLE);  return         seqD_; }
    SeqDouble        & toSeqDouble ()       { check (Type::SEQ_DOUBLE);  return         seqD_; }
    //--------------------------------------------------------------------
    SeqString   const& toSeqString () const { check (Type::SEQ_STRING);  return         seqS_; }
    SeqString        & toSeqString ()       { check (Type::SEQ_STRING);  return         seqS_; }
    //--------------------------------------------------------------------
    SeqMessage  const& toSeqMessage() const { check (Type::SEQ_MESSAGE); return         seqM_; }
    SeqMessage       & toSeqMessage()       { check (Type::SEQ_MESSAGE); return         seqM_; }

    //----------------------------------------------------------------------------
    // Guidelines for the below functions
    //----------------------------------------------------------------------------
    // Indexed array access does not throw exceptions
    // when a precondition is violated. Instead assert is used
    // because here the issue is not part of the Dynamic side of the Library,
    // but the responsability of the library user.
    // Please refer to http://www.boost.org/community/error_handling.html

    //-----------------------------------------------------------------
    bool                toBool  (size_t i, const std::nothrow_t& n) const { assert(i<toSeqBool()  .size());  return toSeqBool   (n)[i]; }
    SeqBool::reference  toBool  (size_t i, const std::nothrow_t& n)       { assert(i<toSeqBool()  .size());  return toSeqBool   (n)[i]; }
    //-----------------------------------------------------------------
    long_t              toLong  (size_t i, const std::nothrow_t& n) const { assert(i<toSeqLong()  .size());  return toSeqLong   (n)[i]; }
    long_t&             toLong  (size_t i, const std::nothrow_t& n)       { assert(i<toSeqLong()  .size());  return toSeqLong   (n)[i]; }
    //-----------------------------------------------------------------
    double              toDouble(size_t i, const std::nothrow_t& n) const { assert(i<toSeqDouble().size());  return toSeqDouble (n)[i]; }
    double&             toDouble(size_t i, const std::nothrow_t& n)       { assert(i<toSeqDouble().size());  return toSeqDouble (n)[i]; }
    //-----------------------------------------------------------------
    std::string const&  toString(size_t i, const std::nothrow_t& n) const { assert(i<toSeqString().size());  return toSeqString (n)[i]; }
    std::string      &  toString(size_t i, const std::nothrow_t& n)       { assert(i<toSeqString().size());  return toSeqString (n)[i]; }
    //-----------------------------------------------------------------
    MSG         const& toMessage(size_t i, const std::nothrow_t& n) const { assert(i<toSeqMessage().size()); return toSeqMessage(n)[i]; }
    MSG              & toMessage(size_t i, const std::nothrow_t& n)       { assert(i<toSeqMessage().size()); return toSeqMessage(n)[i]; }

    //-----------------------------------------------------------------
    bool                toBool  (size_t i)                          const { check (i,toSeqBool()  .size());  return toSeqBool   ()[i]; }
    SeqBool::reference  toBool  (size_t i)                                { check (i,toSeqBool()  .size());  return toSeqBool   ()[i]; }
    //-----------------------------------------------------------------
    long_t              toLong  (size_t i)                          const { check (i,toSeqLong()  .size());  return toSeqLong   ()[i]; }
    long_t&             toLong  (size_t i)                                { check (i,toSeqLong()  .size());  return toSeqLong   ()[i]; }
    //-----------------------------------------------------------------
    double              toDouble(size_t i)                          const { check (i,toSeqDouble().size());  return toSeqDouble ()[i]; }
    double&             toDouble(size_t i)                                { check (i,toSeqDouble().size());  return toSeqDouble ()[i]; }
    //-----------------------------------------------------------------
    std::string const&  toString(size_t i)                          const { check (i,toSeqString().size());  return toSeqString ()[i]; }
    std::string      &  toString(size_t i)                                { check (i,toSeqString().size());  return toSeqString ()[i]; }
    //-----------------------------------------------------------------
    MSG         const& toMessage(size_t i)                          const { check (i,toSeqMessage().size()); return toSeqMessage()[i]; }
    MSG              & toMessage(size_t i)                                { check (i,toSeqMessage().size()); return toSeqMessage()[i]; }

    //----------------------------------------------------------------------------
private:
    template<class T> struct helper { };
    //  template<class T>   get (helper<T>) const;
    bool                get (helper<bool>)               const { return toBool      (); }
    long_t              get (helper<long_t>)             const { return toLong      (); }
    double              get (helper<double>)             const { return toDouble    (); }
    std::string const&  get (helper<std::string const&>) const { return toString    (); }
    MSG         const&  get (helper<MSG         const&>) const { return toMessage   (); }
    SeqBool     const&  get (helper<SeqBool     const&>) const { return toSeqBool   (); }
    SeqLong     const&  get (helper<SeqLong     const&>) const { return toSeqLong   (); }
    SeqDouble   const&  get (helper<SeqDouble   const&>) const { return toSeqDouble (); }
    SeqString   const&  get (helper<SeqString   const&>) const { return toSeqString (); }
    SeqMessage  const&  get (helper<SeqMessage  const&>) const { return toSeqMessage(); }
public:
    template<class T> T get() const { return get(helper<T>()); }

    //--------------------------------------------------------------------------
    void reset ()       {        type_.reset();                }
    Type type  () const { assert(type_.check()); return type_; }
    bool empty () const
    {
        assert(type_.check());
        switch (type_)
        {
        case Type::EMPTY:
            return true;

#ifndef USE_PACKETED_CODING_FOR_SEQUENCE_OF_LENGTH_DELIMITED_ITEMS
        case Type::SEQ_DOUBLE:  return seqD_.empty();
        case Type::SEQ_BOOL:    return seqB_.empty();
        case Type::SEQ_LONG:    return seqL_.empty();
        case Type::SEQ_STRING:  return seqS_.empty();
        case Type::SEQ_MESSAGE: return seqM_.empty();
#endif
        case Type::BOOL:
        case Type::LONG:
        case Type::DOUBLE:
        case Type::STRING:
        case Type::MESSAGE:
        default:
            return false;
        }
    }


    // Exceptions ------------------------------------------
private:

    //----------------------------------------------------------------------------
    void check (size_t i, size_t L) const
    {
        if (i >= L)
        {
            throw OutOfRangeException (type(),i,L);
        }
    }

    //----------------------------------------------------------------------------
    void check (Type req) const
    {
        if (type() != req)
        {
            throw TypeException (type(), req);
        }
    }

public:
    // -----------------------------------------------------------------------
    VariantT& operator = (const VariantT& other)
    {
        type_ = other.type_;
        switch (type_)
        {
        case Type::BOOL:
        case Type::LONG:
        case Type::DOUBLE:      u_    = other.u_;      break;
        case Type::STRING:      strg_ = other.strg_;   break;
        case Type::MESSAGE:     msge_ = other.msge_;   break;
        case Type::SEQ_BOOL:    seqB_ = other.seqB_;   break;
        case Type::SEQ_LONG:    seqL_ = other.seqL_;   break;
        case Type::SEQ_DOUBLE:  seqD_ = other.seqD_;   break;
        case Type::SEQ_STRING:  seqS_ = other.seqS_;   break;
        case Type::SEQ_MESSAGE: seqM_ = other.seqM_;   break;

        default:
            assert(0&&"Unexpected variant type");
        case Type::EMPTY:
            break;
        }
        return *this; // to support chained assignment operators (a=b=c), always return *this
    }

    /*
    // ----------------------------------------------------------------------------
    // Copy constructor
    VariantT (const VariantT& other)
    {
      *this = other;
    }
    */

    // -----------------------------------------------------------------------
    bool operator == (const VariantT& other) const
    {
        if (other.type_ != type_)
            return false;

        switch (type_)
        {
        case Type::BOOL:           return other.u_.bool_ == u_.bool_;
        case Type::LONG:           return other.u_.long_ == u_.long_;
        case Type::DOUBLE:         return other.u_.dble_ == u_.dble_;
        case Type::STRING:         return other.   strg_ ==    strg_;
        case Type::MESSAGE:        return other.   msge_ ==    msge_;
        case Type::SEQ_BOOL:       return other.   seqB_ ==    seqB_;
        case Type::SEQ_LONG:       return other.   seqL_ ==    seqL_;
        case Type::SEQ_DOUBLE:     return other.   seqD_ ==    seqD_;
        case Type::SEQ_STRING:     return other.   seqS_ ==    seqS_;
        case Type::SEQ_MESSAGE:    return other.   seqM_ ==    seqM_;

        default:
            assert(0&&"Unexpected variant type");
        case Type::EMPTY:
            return true;
        }
    }

    // -----------------------------------------------------------------------
    bool operator != (const VariantT& other) const
    {
        return ! (other == *this);
    }

};


//---------------------------------------------------
//bug VC++2010   template <typename T, typename MSG>
//bug VC++2010   T& operator << (T& os, typename pbart::VariantT<MSG>::SEQ const& seq)
template <typename T, typename SEQ>
T& printSeq (T& os, SEQ const& seq)
{
    os << '[';
    for (size_t i=0; i < seq.size(); ++i)
    {
        if (i) os << ',';
        os << seq[i];
    }
    os << ']';
    return os;
}
//---------------------------------------------------
template <typename T, typename SEQ>
T& printStr (T& os, SEQ const& seq)
{
    os <<'['<<'"';
    for (size_t i=0; i < seq.size(); ++i)
    {
        if (i) os <<'"'<<','<<'"';
        size_t p = seq[i].find('"');
        if (p == std::string::npos)
        {
            os << seq[i];
        }
        else
        {
            for (size_t q=0; ; q=p, p=seq[i].find('"',p+1))
            {
                os << seq[i].substr(q,p);
                if (p == std::string::npos)
                    break;
                os << '\\';
            }
        }
    }
    os <<'"'<<']';
    return os;
}
//---------------------------------------------------
template <typename T, typename MSG>
T& print (T& os, const VariantT<MSG>& variant, int n = 0, bool dash = false)
{
    switch (variant.type())
    {
    case pbart::Type::BOOL:    spaces(os,n); os <<     (variant.toBool() ? "true" : "false"); break;
    case pbart::Type::LONG:    spaces(os,n); os <<      variant.toLong();                     break;
    case pbart::Type::DOUBLE:  spaces(os,n); os <<      variant.toDouble();                   break;
    case pbart::Type::STRING:  spaces(os,n); os <<'"'<< variant.toString() <<'"';             break;
    case pbart::Type::SEQ_BOOL:    printSeq (os,        variant.toSeqBool());                 break;
    case pbart::Type::SEQ_LONG:    printSeq (os,        variant.toSeqLong());                 break;
    case pbart::Type::SEQ_DOUBLE:  printSeq (os,        variant.toSeqDouble());               break;
    case pbart::Type::SEQ_STRING:  printStr (os,        variant.toSeqString());               break;
        //TODO(Oliver): SEQ_STRING print is not valid when there is comma in string

    case pbart::Type::MESSAGE:
        print (os, variant.toMessage(), n, dash);
        break;

    case pbart::Type::SEQ_MESSAGE:
    {
        typename VariantT<MSG>::SeqMessage const& seq = variant.toSeqMessage();
        for (size_t i=0; i < seq.size(); ++i)
        {
            print (os, seq[i], n, true);
        }
        break;
    }

    default:
        assert(0 && "Unexpected variant type");
    }
    return os;
}

} // namespace pbart



//---------------------------------------------------
// Generic operator <<
template <typename T, typename MSG>
T& operator << (T& os, const pbart::VariantT<MSG>& variant)
{
    return pbart::print (os, variant);
}


#if 0
//---------------------------------------------------
// Bug in Microsoft Visual C++ 2010 v10
// it requires the Generic operator << be defined within the namespace pbart
namespace pbart
{
template <typename T, typename MSG>
T& operator << (T& os, const VariantT<MSG>& variant)
{
    return ::operator << (os, variant);
}
}
#endif

#endif  // PBART_VARIANT_H_
