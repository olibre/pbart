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
#include "pbart/Message.h"

#include <utility> //pair<>


namespace pbart
{

//--------------------------------------------------------------------------
// Static const Data
const Field Message::fallback_;


//--------------------------------------------------------------------------
Id Message::check (Id id) const
{
    if (id == 0)
    {
        return -1;
    }

    // allowed(id)
    ItemsById::const_iterator itD = items().find(id);
    if (itD == items().end())
    {
        return id;
    }

    // if empty but required => KO
    ValuesById::const_iterator itV = values_.find(id);
    if (itV == values_.end())
    {
        return itD->second.required() ? id : 0; //zero means OK
    }

    // type consistency
    if (itV->second.type() != itD->second.type())
    {
        return id;
    }

    if (itV->second.type() == Type::MESSAGE)
    {
        return itV->second.toMessage(std::nothrow).check();
    }

    if (itV->second.type() == Type::SEQ_MESSAGE)
    {
        const Variant::SeqMessage& s = itV->second.toSeqMessage(std::nothrow);
        for (Variant::SeqMessage::const_iterator it = s.begin(); it !=  s.end(); ++it)
        {
            Id errorId = it->check();
            if (errorId)
            {
                return errorId;
            }
        }
    }

    return 0; //zero means OK
}

//----------------------------------------
Id Message::check() const
{
    assert(def_);
    assert(!dic_ || dic_->id(name())==id());
    assert(!dic_ || dic_->field(id()).name()==name());

    if (id() == 0)
    {
        return -1;
    }

    if (def_->type() != Type::MESSAGE)
    {
        return id();
    }

    for (const_iterator it=begin(); it!=end(); ++it)
    {
        if (it.check())
        {
            return it.id();
        }
    }

    return 0; //zero means OK
}


//--------------------------------------------------------------------------------
void  Message::set (Id id, const Variant& v)
{
    Type type_from_variant = v.type();
    Type type_from_dico    =   type(id); //may throw IdException

    if (type_from_variant != type_from_dico)
        throw TypeException (this->id(), id, type_from_variant, type_from_dico);

    values_[id] =  v;     //values_.insert (ValuesById::value_type (id, v));
    bytes_      = -1;     //TODO(Oliver): may be optimized: unchange size for bool/float/...
}


//--------------------------------------------------------------------------------
// get() const
//--------------------------------------------------------------------------------
const Variant& Message::get (Id id) const
{
    ValuesById::const_iterator it = values_.find(id);
    if (it != values_.end())
    {
        return it->second;
    }

    // fallback = default value
    ItemsById::const_iterator jt = def_->items().find(id);
    if (jt == def_->items().end())
    {
        throw IdException (*this, id);
    }
    else
    {
        return jt->second.defaultValue(); // fallback = default value
    }
}

//--------------------------------------------------------------------------------
// get() mutable
//--------------------------------------------------------------------------------
// This function is usefull for optimization
//--------------------------------------------------------------------------------
Variant& Message::get (Id id)
{
    bytes_ = -1; //reset size because user may change content
    //TODO(Oliver): may be optimized: unchange size for bool/float/...

    ValuesById::iterator it = values_.find(id);
    if (it != values_.end())
    {
        return it->second;
    }

    // fallback = default value
    ItemsById::const_iterator jt = def_->items().find(id);
    if (jt == def_->items().end())
    {
        throw IdException (*this, id);
    }
    else
    {
        // map::insert() returns a 'pair'
        std::pair<ValuesById::iterator,bool> pair = values_.insert (ValuesById::value_type (id, jt->second.defaultValue()));
        assert (pair.second); // when 'pair.second' is 'false' => key was already inserted
        return pair.first->second;
    }
}

//--------------------------------------------------------------------------------
Message::iterator Message::begin()
{
    ValuesById::iterator b = values_.begin();
    ValuesById::iterator e = values_.end();

#ifdef USE_DIRECT_POINTER_ACCESS
    return iterator (items().begin(), b, e);
#else
    return iterator (items().begin(), b, e, dico_);
#endif
}

//--------------------------------------------------------------------------------
Message::const_iterator  Message::begin()  const
{
    ItemsById ::const_iterator i = items().begin();
    ValuesById::const_iterator b = values_.begin();
    ValuesById::const_iterator e = values_.end();

#ifdef USE_DIRECT_POINTER_ACCESS
    return const_iterator (i, b, e);
#else
    return const_iterator (i, b, e, dico_);
#endif
}

//--------------------------------------------------------------------------------
bool Message::operator == (const Message& m) const
{
    // if pointers of dico are same => continue
    // else if one of the two pointers is null or content of dico are different => return false
    // TODO(Oliver): What same message but using dico different in other parts
    //      case of: Message() != Serializer::message()
    //      or case: Message() != Serializer::decode("")
    if (m.dic_ != dic_  &&  (!m.dic_ || !dic_ || *m.dic_ != *dic_))
    {
        return false;
    }

    if (m.id_ != id_)
    {
        return false;
    }

    if (m.values_.size() != values_.size())
    {
        return false;
    }

    for (ValuesById::const_iterator i  = values_.begin(), j = m.values_.begin();
         i != values_.end();
         ++i, ++j)
    {
        if (*i != *j) //see Variant operator ==
            return false;
    }

    return true;
}

//--------------------------------------------------------------------------
// removes empty sequences
void Message::trim()
{
    for (ValuesById::iterator it = values_.begin(); it != values_.end(); )
    {
        Variant& v = it->second;
        switch (v.type())
        {
        case Type::EMPTY:
            break; // break => erase 'it' after the switch block

        case Type::SEQ_DOUBLE:  if (v.toSeqDouble (std::nothrow).empty()) break; goto no_erase;
        case Type::SEQ_BOOL:    if (v.toSeqBool   (std::nothrow).empty()) break; goto no_erase;
        case Type::SEQ_LONG:    if (v.toSeqLong   (std::nothrow).empty()) break; goto no_erase;
        case Type::SEQ_STRING:  if (v.toSeqString (std::nothrow).empty()) break; goto no_erase;
        case Type::SEQ_MESSAGE: if (v.toSeqMessage(std::nothrow).empty()) break;
        {
            Variant::SeqMessage& seq = v.toSeqMessage(std::nothrow);
            for (Variant::SeqMessage::iterator m = seq.begin(); m != seq.end(); ++m)
            {
                m->trim();
            }
            goto no_erase;
        }

        case Type::MESSAGE:
            v.toMessage(std::nothrow).trim();
            goto no_erase;

        default:
        case Type::BOOL:
        case Type::LONG:
        case Type::DOUBLE:
        case Type::STRING:
no_erase:
            ++it;
            continue;  // continue => do not erase 'it'
        }

        values_.erase(it++);
    }
}


//--------------------------------------------------------------------------------
size_t Message::erase (Id id)
{
    return values_.erase (id);
}


//--------------------------------------------------------------------------------
void Message::erase (Message::iterator& it)
{
    //assert(bytes_); //it may already been erased => bytes_ may be zero
    it.erase (values_);
    bytes_ = -size(); //bytes_ = -1; //TODO(Oliver):may be optimized: decrement the size of the erased item
}

} // namespace pbart
