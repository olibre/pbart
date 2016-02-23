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

#ifndef PBART_DICO_H_
#define PBART_DICO_H_


#define USE_DIRECT_POINTER_ACCESS


#include <map>
#include <string>
#include <vector>
#include <cassert>

#include "pbart/Types.h"
#include "pbart/Exception.h"


namespace pbart
{
//------------------------------------------------------------------------------
struct Header
{
    void init (int version, uint_fast16_t checksum);
    std::string str;

    bool operator == (const Header& other) const { return other.str == str;   }
    bool operator != (const Header& other) const { return ! (*this == other); }
};

//------------------------------------------------------------------------------
// forward declaration required by ItemT
template <typename V>
class FieldT;

//------------------------------------------------------------------------------
template <typename V>
class ItemT
{
public:
    ItemT           (const ItemT& o) : variant_(o.variant_), p_(o.p_), required_(o.required_) {}
    ItemT& operator=(const ItemT& o) { variant_=o.variant_;  p_=o.p_;  required_=o.required_; return *this; }
    explicit ItemT (bool r) : required_(r)
    {
#ifdef USE_DIRECT_POINTER_ACCESS
        p_ = 0;
#endif
    }

#ifndef NDEBUG  // NDEBUG => This operator== is used to control Dico::operator==() in debug mode
    bool operator == (const ItemT& other) const;
    bool operator != (const ItemT& other) const { return ! (*this == other); }
#endif

    // Member 'default value'
public:
    V const&  defaultValue() const { return variant_; }  //get
    V      &  defaultValue()       { return variant_; }  //set
private:
    V  variant_; //variant

    // Direct access to fields
public:
#ifdef USE_DIRECT_POINTER_ACCESS
    typedef FieldT<V> Field;
    const Field* ptr() const { assert(p_); return p_; } //get  //when Serializer::pointers() is not called => p_ is null
    void         ptr(const Field* p)  { p_ = p; }       //set
private:
    const Field* p_;    // set at the end of Dico parsing
#else
    void         ptr(const Field* p)  { }       //set
#endif

    // Member 'required'
public:
    bool  required() const { return required_; }
private:
    bool  required_;    // required (true) or optional (false)

    // Members 'type' and 'repeated' => use the member 'type' from variant
public:
    Type  type()     const { return variant_.type();            }
    bool  repeated() const { return variant_.type().repeated(); }
};


//------------------------------------------------------------------------------
template <typename V>
class FieldT
{
public:
    FieldT() : name_(), comment_(), type_(Type::EMPTY), deprecated_(true) {}
    typedef ItemT<V> Item;

    //-get----------------------------------------------------
    Type                type      ()  const { return type_;       } //typeDico
    const std::string&  name      ()  const { return name_;       }
    const std::string&  comment   ()  const { return comment_;    }
    bool                deprecated()  const { return deprecated_; }

    //-set----------------------------------------------------
    void  type       (Type t)               { type_       = t; }  //typeDico
    void  name       (const std::string& n) { name_       = n; }
    void  comment    (const std::string& c) { comment_    = c; }
    void  deprecated (bool d)               { deprecated_ = d; }

private:
    std::string  name_;
    std::string  comment_;

    // for type = MESSAGE
public:
    typedef std::map <Id, Item>  ItemsById;   //typedef
private:
    ItemsById    items_;                      //data

    Type         type_; //typeDico
    bool         deprecated_;

public:
    //--functions---------------------------------------------

    Item* add (Id, bool);

    ItemsById const& items() const { return items_; }
    ItemsById      & items()       { return items_; }

    // -------------------------------------------------------
    bool allowed (Id id) const
    {
        return items().find(id) != items().end();
    }

    // -------------------------------------------------------
    Item const& item (Id id) const
    {
        typename ItemsById::const_iterator  it = items_.find(id);
        if (it == items_.end())
        {
            throw  IdException(*this,id);
        }
        return it->second;
    }

    // -------------------------------------------------------
    Item& item (Id id)
    {
        typename ItemsById::iterator  it = items_.find(id);
        if (it == items_.end())
        {
            throw  IdException(*this,id);
        }
        return it->second;
    }

#ifndef NDEBUG // NDEBUG => This operator== is used to control Dico::operator==() in debug mode
    // -------------------------------------------------------
    bool operator == (const FieldT& other) const;
    bool operator != (const FieldT& other) const { return ! (*this == other); }
#endif
};


//------------------------------------------------------------------------------
template <typename V>
class DicoT
{
    //private: //disable copy because of Field->item[id]->ptr => use swap()
    //  DicoT (const DicoT&);
    //  DicoT& operator = (const DicoT&);
public:
    DicoT();
    void pointers();
    void swap (DicoT& other);

    // -------------------------------------------------------
    bool operator == (const DicoT& other) const;
    bool operator != (const DicoT& other) const { return ! (*this == other); }

    // Checksum & Version
public:
    void  version  (int version)       { version_ = version;        } //set
    int   version  ()           const  { return version_;           } //get
    const std::string& header() const  { return header_.str;        } //get
    void  checksum (uint_fast16_t cs)  { header_.init(version_,cs); } //set

    // Fields & Messages
    //---types----
public:
    typedef FieldT<V> Field;
    typedef std::map <std::string, Id>  IdByName;
    typedef std::vector <Field>         FieldById;
    //---get---
public:
    Id   id (const std::string& name)                        const;
    //  Id   id (const std::string& name, const std::nothrow_t&) const;
    //-------
    //  Field const&   operator[](Id id)                         const;
    Field const&   field     (Id id)                         const;
    //  Field const&   field     (Id id, const std::nothrow_t&)  const;
    //-------
    FieldById const&  fields() const { return fieldById_; }
    FieldById      &  fields()       { return fieldById_; }
    //-------
    IdByName const&   names()  const { return idByName_; }
    //---set---
public:
    // ------------------------------------------------------------------------
    // idByName_[name] = id
    Id     addName  (Id id, const std::string& name);
    Field& addField (Id id, const std::string& name);

    // add any new attribute in Dico::swap()

    //---data---
private:
    Header     header_;
    IdByName   idByName_;
    FieldById  fieldById_;
    int        version_;
};

} // namespace pbart


// INLINE


namespace pbart
{
//--------------------------------------------------------------------------
// helper functions
//--------------------------------------------------------------------------

// template <typename T>
// bool exist (const T& m, const typename T::key_type& k)
// {
//   return (m.find(k) != m.end());
// }

// ------------------------------------------------------------------------
inline void  Header::init (int version, uint_fast16_t checksum)
{
    str.resize (3);
    str[0] =  version;
    str[1] = (checksum & 0xFF00) >> 8;
    str[2] =  checksum & 0x00FF;
}

// ------------------------------------------------------------------------
template <typename V>
DicoT<V>::DicoT()
{
    version_ = 0;

    // avoid exception while decoding buffer containing an empty message using id=0
    addName (0, std::string());
    fieldById_.resize (1); //similar to: add (0,Field());
}


// ------------------------------------------------------------------------
template <typename V>
void DicoT<V>::swap (DicoT& other)
{
    std::swap (this->version_,   other.version_);
    std::swap (this->header_,    other.header_);
    std::swap (this->idByName_,  other.idByName_);
    std::swap (this->fieldById_, other.fieldById_);
}


// ------------------------------------------------------------------------
// template <typename V>
// Id DicoT<V>::id (const std::string& name)  const
// {
//   IdByName::const_iterator it = idByName_.find(name);
//   if (it == idByName_.end())   return throw NameException(name);
//   else                         return it->second;
// }


// ------------------------------------------------------------------------
template <typename V>
Id DicoT<V>::id (const std::string& name/*, const std::nothrow_t&*/)  const
{
    IdByName::const_iterator it = idByName_.find(name);

    if (it == idByName_.end())
    {
        return 0; //zero means 'not found'
    }
    else
    {
        return it->second;
    }
}

// These operator==() are used to control Dico::operator==() in debug mode
#ifndef NDEBUG
// ------------------------------------------------------------------------
template <typename V>
bool ItemT<V>::operator == (const ItemT& other) const
{
    if (other.required_ != required_)
    {
        return false;
    }

    // Do not call Variant::operator== for Type::MESSAGE because it may recursively call Dico::operator==
    if (variant_.type().primitive())
    {
        return other.variant_ == variant_;
    }
    else
    {
        return other.variant_.type() == variant_.type();
    }
}

// ------------------------------------------------------------------------
template <typename V>
bool FieldT<V>::operator == (const FieldT& other) const
{
    return other.type_  == type_ &&
            other.name_  == name_ &&
            other.items_ == items_;
}
#endif

// -------------------------------------------------------
template <typename V>
bool DicoT<V>::operator == (const DicoT& other) const
{
    assert( (other.header_  == header_) == (other.fieldById_ == fieldById_) );

    return other.header_    == header_;// &&
    //other.fieldById_ == fieldById_ &&    //no need to check fieldById_
    //other.idByName_  == idByName_;
}



// //------------------------------------------------------------------------------
// template <typename V>
// const FieldT<V>&  DicoT<V>::field (Id id) const
// {
//   if (id >= fieldById_.size() || (id && fieldById_[id].type() == Type::EMPTY))
//   {
//     throw IdException(id);
//   }
//
//   return fieldById_[ id ];
// }


//------------------------------------------------------------------------------
template <typename V>
const FieldT<V>&  DicoT<V>::field (Id id/*, const std::nothrow_t&*/) const
{
    if (id >= fieldById_.size())
    {
        return fieldById_[ 0 ];
    }

    return fieldById_[ id ];
}


// ------------------------------------------------------------------------------
// template <typename V>
// const FieldT<V>&  DicoT<V>::operator[] (Id id) const
// {
//   assert(id >= 0);
//   assert(size_t(id) < fieldById_.size());
//   assert(id==0 || fieldById_[id].type() != Type::EMPTY);
//
//   return fieldById_[ id ];
// }


// ------------------------------------------------------------------------
template <typename V>
pbart::ItemT<V>* FieldT<V>::add (Id id, bool required)
{
    // map::insert() returns a 'pair'
    // when 'pair.second' is 'false' => key was already inserted
    std::pair <typename ItemsById::iterator, bool> pair =
            items().insert (typename ItemsById::value_type (id, Item(required)));

    if (pair.second)
    {
        return & (pair.first->second);
    }
    else
    {
        return 0;
    }
}

// ------------------------------------------------------------------------
// idByName_[name] = id
template <typename V>
Id DicoT<V>::addName (Id id, const std::string& name)
{
    // map::insert() returns a 'pair'
    // when 'pair.second' is 'false' => key was already inserted
    std::pair <IdByName::iterator, bool> pair =
            idByName_.insert (IdByName::value_type (name, id));

    if (pair.second)
    {
        return 0;
    }
    else
    {
        return pair.first->second;
    }
}

// ------------------------------------------------------------------------
// Helper
template <typename T>
static void enlarge (T& vector, size_t index)
{
    if (vector.size() <= index)
        vector.resize( index + 1 );
}

// ------------------------------------------------------------------------
template <typename V>
pbart::FieldT<V>& DicoT<V>::addField (Id id, const std::string& name)
{
    assert( id > ID_MIN );
    assert( id < ID_MAX );

    Id  otherId = addName (id, name);
    if (otherId) // duplicated name
    {
        std::ostringstream oss;
        oss <<"Duplicated name='"<< name <<"' used by id="<< id <<" and id="<< otherId;
        const std::string& str = oss.str();
        throw DicoException (str);
    }

    enlarge (fieldById_, id);
    Field& dest = fieldById_.at(id);
    if (dest.type() != Type::EMPTY) //duplicated Id
    {
        std::ostringstream oss;
        oss <<"Duplicated id="<< id <<" used by fields '"<< name <<"' and '"<< dest.name() <<'\'';
        const std::string& str = oss.str();
        throw DicoException (str);
    }

    // set name
    assert(dest.name().empty());
    dest.name(name);

    return dest; // dest must be filled
}

} // namespace pbart



#endif  // PBART_DICO_H_
