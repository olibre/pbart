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

#ifndef PBART_ITERATOR_H_
#define PBART_ITERATOR_H_


#include "pbart/Types.h"
#include "pbart/Dico.h"
#include "pbart/Variant.h"

#include <boost/shared_ptr.hpp>
#include <string>


namespace pbart
{
//------------------------------------------------------------------------
template <typename MSG, typename VIt> //ValuesById::iterator or ValuesById::const_iterator
class internal_common_iteratorT
{
    // Types
protected:
    typedef VariantT<MSG>    Variant;
    typedef FieldT<Variant>  Field;
    typedef DicoT <Variant>  Dico;
    typedef typename Field::ItemsById::const_iterator DIt;

    // constructors used by Message::begin() and Message::end()
public:
#ifdef USE_DIRECT_POINTER_ACCESS
    explicit internal_common_iteratorT (DIt dic, VIt var, VIt end)                     : dic_(dic),  var_(var), end_(end)           {}
#else
    explicit internal_common_iteratorT (DIt dic, VIt var, VIt end, const SharedDico d) : dic_(item), var_(var), end_(end), dico_(d) {}
#endif
    explicit internal_common_iteratorT (DIt dic)                                       : dic_(dic),  var_(),    end_()              {}

public:
    bool          deprecated() const { return item()->deprecated();     }
    bool            required() const { return dic_->second.required();  }
    bool            repeated() const { return dic_->second.repeated();  }
    const std::string&  name() const { return item()->name();           }
    Id                    id() const { return dic_->first;              }
    Type            type    () const { return dic_->second.type();      }
    Type            typeDico() const { return item() -> type();         }

    bool operator==(const internal_common_iteratorT& it) const { return dic_ == it.dic_; }
    bool operator!=(const internal_common_iteratorT& it) const { return dic_ != it.dic_; }
    internal_common_iteratorT& operator++()                    { ++dic_; return *this;   }

    Variant const& operator* () const { if (miss()) return  dic_->second.defaultValue(); return  var_->second; }
    Variant const* operator->() const { if (miss()) return &dic_->second.defaultValue(); return &var_->second; }


    //-------------------------------------------------------------------------
    Id check() const
    {
        assert (id());

        // if empty but required => KO
        if (empty())
        {
            return required() ? id() : 0; //zero means OK
        }

        // type consistency
        if (type() != var_->second.type())
        {
            return id();
        }

        if (type() == Type::MESSAGE)
        {
            return var_->second.toMessage(std::nothrow).check();
        }

        if (type() == Type::SEQ_MESSAGE)
        {
            const typename Variant::SeqMessage& s = var_->second.toSeqMessage(std::nothrow);
            for (typename Variant::SeqMessage::const_iterator it = s.begin(); it != s.end(); ++it)
            {
                Id error = it->check();
                if (error)
                {
                    return error;
                }
            }
        }

        return 0; //zero means OK
    }


    //-------------------------------------------------------------------------
    // Variant containing a Sequence without any elements is considered as empty
    // (see USE_PACKETED_CODING_FOR_SEQUENCE_OF_LENGTH_DELIMITED_ITEMS)
    bool empty() const
    {
        return miss() ? true : var_->second.empty();
    }

    //-------------------------------------------------------------------------
    // Variant containing a Sequence without any elements is considered as present
    bool present() const
    {
        return miss() ? false : !var_->second.type().empty();
    }

protected:
    //-------------------------------------------------------------------------
    bool miss() const
    {
        do
        {
            if (var_ == end_)
                return true;
        }
        while (inc());
        return (var_->first != dic_->first);
    }

private:
    //-------------------------------------------------------------------------
    bool inc() const
    {
        if (var_->first >= dic_->first)
            return false;
        ++var_;
        return true;
    }

#ifdef USE_DIRECT_POINTER_ACCESS
    const Field* item() const { return dic_->second.ptr(); }
#else
    const Field* item() const { return dic_->item(id()); }
    const Dico*  dic_;
#endif

    // Data
protected:
    DIt          dic_;  //iterator on Dico properties
    VIt  mutable var_;  //iterator on Values (mutable because const functions may increment this variable)
    VIt          end_;  //was const, but class could not use default operator=    see error: non-static const member 'const std::_Rb_tree_iterator<std::pair<const long unsigned int, pbart::VariantT<pbart::Message> > > pbart::internal_common_iteratorT<pbart::Message, std::_Rb_tree_iterator<std::pair<const long unsigned int, pbart::VariantT<pbart::Message> > > >::end_', can't use default assignment operator
}; //class internal_common_iteratorT

//----------------------------------------------------------------------
template <typename MSG>
class const_iteratorT : public internal_common_iteratorT <MSG, typename MSG::ValuesById::const_iterator>
{
public:
    typedef          VariantT<MSG>                    Variant;
    typedef          FieldT<Variant>                  Field;
    typedef typename Field::ItemsById::const_iterator  DIt;
    typedef typename MSG::ValuesById::const_iterator VIt;

    // constructors used by Message::begin() and Message::end()
#ifdef USE_DIRECT_POINTER_ACCESS
    explicit const_iteratorT (DIt dic, VIt var, VIt end)               : internal_common_iteratorT<MSG,VIt> (dic, var, end)    { }
#else
    explicit const_iteratorT (DIt dic, VIt var, VIt end, const Dico* d): internal_common_iteratorT<MSG,VIt> (dic, var, end, d) { }
#endif

    explicit const_iteratorT (DIt dic) : internal_common_iteratorT<MSG,VIt> (dic) { }
};

//-----------------------------------------------------------------------
template <typename MSG>
class iteratorT : public internal_common_iteratorT <MSG, typename MSG::ValuesById::iterator>
{
public:
    typedef          VariantT<MSG>                   Variant;
    typedef          FieldT<Variant>                 Field;
    typedef typename Field::ItemsById::const_iterator DIt;
    typedef typename MSG::ValuesById::iterator        VIt;

    // constructors used by Message::begin() and Message::end()
#ifdef USE_DIRECT_POINTER_ACCESS
    explicit iteratorT (DIt dic, VIt var, VIt end)               : internal_common_iteratorT<MSG,VIt> (dic, var, end)    { }
#else
    explicit iteratorT (DIt dic, VIt var, VIt end, const Dico* d): internal_common_iteratorT<MSG,VIt> (dic, var, end, d) { }
#endif

    explicit iteratorT (DIt dic) : internal_common_iteratorT<MSG,VIt> (dic) { }

public:
    Variant& operator* () { if (this->miss()) throw EmptyException(this->id());  return  this->var_->second; }
    Variant* operator->() { if (this->miss()) throw EmptyException(this->id());  return &this->var_->second; }

    // -----------------------------------------------------------------------
    void erase (typename MSG::ValuesById& container)
    {
        if (! this->miss())
        {
            //in C++03: get the incremented iterator before calling erase => post increment withing the erase call: erase(it++)
            container.erase (this->var_++);
            //in C++11: map.erase(it) returns the next iterator as for vector.map(it)
            // var_ = container.erase (var_);
        }
    }
};


//-------------------------------------------------------------------------
template <typename T, typename MSG, typename VIt>
T& print (T& os, const internal_common_iteratorT<MSG,VIt>& it, int n = 0, bool dash = false)
{
    bool _empty = it.empty(); //increment sub-iterator var_

    if (it.typeDico() == pbart::Type::MESSAGE)
    {
        if (it.repeated())
        {
            spaces(os,n);  os << it.name() <<':'<<'\n';
        }
        if (!_empty)
        {
            print (os, *it, n, dash);
        }
    }
    else
    {
        spaces(os,n);
        if (dash)
        {
            os <<'-'<<' ';
            n += 2;
        }
        os << it.name() <<':'<<'\n';

        spaces(os,n);    os <<"  id:    "<< it.id()         <<'\n';
        spaces(os,n);    os <<"  type:  "<< it.type().str() <<'\n';

        if (!_empty)
        {
            spaces(os,n);  os <<"  value: "<< *it <<'\n';
        }
    }

    return os;
}

} //namespace pbart

//-------------------------------------------------------------------------
// Generic operator <<
template <typename T, typename MSG, typename VIt>
T& operator << (T& os, const pbart::internal_common_iteratorT<MSG,VIt>& it)
{
    return pbart::print (os, it);
}


#endif  // PBART_ITERATOR_H_
