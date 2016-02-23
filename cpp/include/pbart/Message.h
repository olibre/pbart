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

#ifndef PBART_MESSAGE_H_
#define PBART_MESSAGE_H_

#include <map>
#include <string>

#include "pbart/Dico.h"
#include "pbart/Iterator.h"


namespace pbart
{
// ------------------------------------------------------------------------
class PBART_EXPORT Message
{
    // ---------------------------------------------------------
    // Variant
public:
    typedef  VariantT<Message>             Variant;
    typedef  DicoT   <Variant>             Dico;
    typedef  Dico::Field                   Field;
    typedef  Field::Item                   Item;
    typedef  Field::ItemsById              ItemsById;
    typedef  std::map<Id, Variant>         ValuesById;
    typedef  boost::shared_ptr<const Dico> SharedDico;

public:
    Message ();
    Message (const Message&);
    Message& operator=(const Message&);
    explicit Message (const SharedDico& d);
    Message (const SharedDico& d, Id id);

    // Iterator
public:
    typedef const_iteratorT<Message>  const_iterator;
    typedef       iteratorT<Message>        iterator;
    const_iterator  cbegin() const;
    const_iterator  cend  () const;
    const_iterator  begin () const;
    const_iterator  end   () const;
    iterator        begin ();
    iterator        end   ();

    // Functions
    Type                 type ()   const;
    const std::string&   name ()   const;
    const std::string&   name (Id) const;
    void                  set (Id, const Variant& v);
    Variant const&        get (Id) const;
    Variant      &        get (Id);
    Variant const& operator[] (Id) const;
    //  Variant      & operator[] (Id);
    Type                 type (Id) const;
    Type             typeDico (Id) const;
    bool           deprecated (Id) const;
    bool             required (Id) const;
    bool             repeated (Id) const;
    bool              allowed (Id) const;
    bool                empty (Id) const;
    size_t              erase (Id);
    void                erase (iterator&);
    Id                  check (Id) const;
    Id                  check ()   const;
    Id                     id ()   const;
    Id                     id (const std::string&) const;
    void                   id (Id);
    void                 trim ();
    void                clear ();
    int                  size ()   const;
    bool                empty ()   const;
    ItemsById  const&   items ()   const;
    SharedDico const&    dico ()   const;

    bool operator == (const Message& m) const;
    bool operator != (const Message& m) const;


    // Data ---------------------------------
private:
    Id            id_;
    SharedDico    dic_;
    const Field*  def_;
    ValuesById    values_;
    mutable int   bytes_;   // size of the serialized message

    // ------ Static const Data -----------
    static const Field  fallback_;


    friend class Codec;
};

// ---------------------------------------------------------
typedef  Message::Variant      Variant;
typedef  Message::Field        Field;
typedef  Message::Item         Item;
typedef  Message::Dico         Dico;
typedef  Field::ItemsById      ItemsById;
typedef  Message::ValuesById   ValuesById;
typedef  Message::SharedDico   SharedDico;

}  // namespace pbart


// INLINE


namespace pbart
{
//--------------------------------------------------------------------------
inline Message::Message ()                           : id_(0),      dic_(0),      def_(&fallback_),                     values_(),          bytes_(0)        { assert(def_->items().empty()); }
inline Message::Message (const Message& o)           : id_(o.id_),  dic_(o.dic_), def_(o.def_),                         values_(o.values_), bytes_(o.bytes_) {                                }
inline Message& Message::operator=(const Message& o) { id_=o.id_;   dic_=o.dic_;  def_=o.def_;                          values_=o.values_;  bytes_=o.bytes_;   return *this;                  }
inline Message::Message (const SharedDico& d)        : id_(0),      dic_(d),      def_(&fallback_),                     values_(),          bytes_(0)        { assert(def_->items().empty()); }
inline Message::Message (const SharedDico& d, Id id) : id_(id),     dic_(d),      def_(d ? &d->field(id) : &fallback_), values_(),          bytes_(0)
{
#if !defined(COMPILING_CPPUNIT_CODEC) && !defined(COMPILING_CPPUNIT_MESSAGE) && !defined (_MSC_VER) //bug in MS Visual C++ In this case; it ignores COMPILING_CPPUNIT_MESSAGE has been defined before including headers.
    assert(def_->type() == Type::MESSAGE);
#endif
}

//--------------------------------------------------------------------------
//                             Message::begin     () defined in Message.cpp
inline Message::const_iterator Message::cbegin    ()      const { return begin();                             }
inline Message::const_iterator Message::cend      ()      const { return end();                               }
inline Message::const_iterator Message::end       ()      const { return const_iterator (items().end());      }
inline Message::iterator       Message::end       ()            { return       iterator (items().end());      }
inline int                     Message::size      ()      const { return values_.size();                      }
inline bool                    Message::empty     ()      const { return values_.empty();                     }
inline Id                      Message::id        ()      const { return id_;                                 }
inline const ItemsById&        Message::items     ()      const { return def_->items();                       }
inline const std::string&      Message::name      ()      const { return def_->name();                        }
inline Type                    Message::type      ()      const { return def_->type();                        }
inline Type                    Message::type      (Id id) const { return def_->item(id).type();               }
inline bool                    Message::allowed   (Id id) const { return def_->allowed(id);                   }
inline bool                    Message::required  (Id id) const { return def_->item(id).required();           }
inline bool                    Message::repeated  (Id id) const { return def_->item(id).repeated();           }
inline Type                    Message::typeDico  (Id id) const { return dic_ ? dic_->field(id).type() : Type();           } //Type::EMPTY
inline const std::string&      Message::name      (Id id) const { return dic_ ? dic_->field(id).name() : fallback_.name(); }
inline bool                    Message::deprecated(Id id) const { return dic_ ? dic_->field(id).deprecated() : true;       }

//--------------------------------------------------------------------------
// function  set() and get() defined in Message.cpp
inline const Variant&  Message::operator[](Id id)            const { return get(id);       }
//inline       Variant&  Message::operator[](Id id)                  { return get(id);       }
inline bool            Message::operator!=(const Message& m) const { return !(*this == m); }
//                     Message::operator==  defined in Message.cpp

//--------------------------------------------------------------------------
inline bool Message::empty (Id id) const
{
    ValuesById::const_iterator it = values_.find(id);
    return (it == values_.end()) || it->second.empty();
}

//--------------------------------------------------------------------------
inline const SharedDico&
Message::dico() const
{
    return dic_;
}

//--------------------------------------------------------------------------
inline void Message::id (Id id)
{
    id_ = id;
    if (dic_)
    {
        def_ = & dic_->field(id);
    }
}

//--------------------------------------------------------------------------
inline Id Message::id (const std::string& name) const
{
    if (dic_)
    {
        return dic_->id(name);
    }
    return 0;
}

//--------------------------------------------------------------------------------
inline void Message::clear()
{
    values_.clear(); //TODO(Oliver): check if vectors within variants are well cleared...
    bytes_ = 0;
    assert(!size());
}

//--------------------------------------------------------------------------
template <typename T>
T& print (T& os, const Message& msg, int n = 0, bool dash = false)
{
    if (!dash)
    {
        spaces(os,n); os <<              msg.name() <<':'<<'\n';
        spaces(os,n); os <<"  id:    "<< msg.id()        <<'\n';
    }

    // if message is not empty (does not contain any value)
    if (! msg.empty())
    {
        Message::const_iterator it = msg.begin();
        assert(it != msg.end());

        if (dash)
        {
            print (os, it, n, true);
            ++it;
        }

        n += 2;
        for (; it != msg.end(); ++it)
            print (os, it, n, false);
    }

    return os;
}


} //namespace pbart

//--------------------------------------------------------------------------
// Generic operator <<
template <typename T>
T& operator << (T& os, const pbart::Message& msg)
{
    return pbart::print (os, msg);
}


#endif  // PBART_MESSAGE_H_
