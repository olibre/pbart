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
#include "pbart/Serializer.h"
#include "pbart/Dico.h"
#include "pbart/Message.h"
#include "pbart/DicoXMLReader.h"
#include "pbart/Codec.h"

#include <google/protobuf/wire_format.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h> //StringOutputStream

#include <string>


namespace pbart
{
//--------------------------------------------------------------------------
// static attributes
Id Serializer::idId_;
Id Serializer::nameId_;
Id Serializer::typeId_;
Id Serializer::deprecatedId_;
Id Serializer::commentId_;
Id Serializer::requiredId_;
Id Serializer::defaultDId_;
Id Serializer::defaultSId_;
Id Serializer::versionId_;
Id Serializer::itemId_;
Id Serializer::fieldId_;
//Id Serializer::dicoId_;
const std::string Serializer::nodico_("NoDico"); // exit-time destructor


//--------------------------------------------------------------------------
void Serializer::loadDico (const std::string& file)
{
    Dico* ptr;
    SharedDico newDico( ptr = new Dico() );
    DicoXMLReader(*ptr).load( file.c_str() );

    pointers (ptr, newDico); // Reset Dico internal pointers
    dico_ = newDico;         // Replace current dico by the new one
}

//--------------------------------------------------------------------------
void Serializer::parseDico (const char* xml)
{
    Dico* ptr;
    SharedDico newDico( ptr = new Dico() );
    DicoXMLReader(*ptr).parse(xml);

    pointers (ptr, newDico); // Reset Dico internal pointers
    dico_ = newDico;         // Replace current dico by the new one
}

//--------------------------------------------------------------------------
void Serializer::msgToDico (const Message& msg)
{
    const SharedDico& dod = dicoOfDico();

    if (*msg.dico() != *dod)
    {
        std::ostringstream oss;
        oss <<"Cannot extract Dictionary from message '"<< msg.name() <<"' (id="<< msg.id() <<") "
                                                                                              "because it is based on a Dictionary which is not reconized as a DicoOfDico";
        const std::string& str = oss.str();
        throw DicoException (str);
    }

    if (dicoId_ != msg.id())
    {
        std::ostringstream oss;
        oss <<"Cannot extract Dictionary from message '"<< msg.name() <<"' because its id="<< msg.id() <<" must be "<< dicoId_;
        const std::string& str = oss.str();
        throw DicoException (str);
    }

    Dico* ptr;
    SharedDico newDico( ptr = new Dico() );
    // Version
    ptr->version( msg.get(versionId_).toLong() );
    // Fields and Messages
    //if (! msg.get(fieldId_).empty())
    {
        const Variant::SeqMessage& fields = msg.get(fieldId_).toSeqMessage();
        for (Variant::SeqMessage::const_iterator it = fields.begin(); it != fields.end(); ++it)
        {
            addField (it, ptr);
        }
    }

    pointers (ptr, newDico); // Reset Dico internal pointers
    dico_ = newDico;         // Replace current dico by the new one
}

// -------------------------------------------------------------------------
// lasy initialization
const SharedDico& Serializer::dicoOfDico()
{
    static SharedDico dod; // exit-time destructor

    //lazy initialization
    if (! dod)
    {
        Dico* ptr;
        dod = SharedDico( ptr = new Dico() );
        DicoXMLReader(*ptr).parse("<?xml version='1.0' encoding='UTF-8' ?>"
                                  "<Dico version='1.0.0' xsd='1.0.0' env='dev'>"
                                  "<FieldsDef>"
                                  "  <FieldDef id='2'  name='id'         type='long'   deprecated='false' comment='' />"
                                  "  <FieldDef id='3'  name='name'       type='string' deprecated='false' comment='' />"
                                  "  <FieldDef id='4'  name='type'       type='long'   deprecated='false' comment='' />"
                                  "  <FieldDef id='5'  name='deprecated' type='bool'   deprecated='false' comment='' />"
                                  "  <FieldDef id='6'  name='comment'    type='string' deprecated='false' comment='' />"
                                  "  <FieldDef id='7'  name='required'   type='bool'   deprecated='false' comment='' />"
                                  "  <FieldDef id='8'  name='defaultD'   type='double' deprecated='false' comment='' />"
                                  "  <FieldDef id='9'  name='defaultS'   type='string' deprecated='false' comment='' />"
                                  "  <FieldDef id='10' name='version'    type='long'   deprecated='false' comment='' />"
                                  "</FieldsDef>"
                                  "<MessagesDef>"
                                  "  <MessageDef id='12' name='Item' deprecated='false' comment=''>"
                                  "    <Field name='required' required='false' repeated='false' default='false' />"
                                  "    <Field name='id'       required='true'  repeated='false' default='0'     />"
                                  "    <Field name='type'     required='true'  repeated='false' default='0'     />"
                                  "    <Field name='defaultD' required='false' repeated='false' default='0'     />"
                                  "    <Field name='defaultS' required='false' repeated='false' default=''      />"
                                  "  </MessageDef>"
                                  "  <MessageDef id='13' name='Field' deprecated='false' comment=''>"
                                  "    <Field   name='id'         required='true'  repeated='false' default='0'     />"
                                  "    <Field   name='name'       required='true'  repeated='false' default=''      />"
                                  "    <Field   name='type'       required='true'  repeated='false' default='0'     />"
                                  "    <Field   name='deprecated' required='false' repeated='false' default='false' />"
                                  "    <Field   name='comment'    required='false' repeated='false' default=''      />"
                                  "    <Message name='Item'       required='false' repeated='true'                  />"
                                  "  </MessageDef>" //id of Dico must be '1'
                                  "  <MessageDef id='1' name='Dico' deprecated='false' comment=''>"
                                  "    <Field   name='version' required='false' repeated='false' default='0' />"
                                  "    <Message name='Field'   required='false' repeated='true' />"
                                  "  </MessageDef>"
                                  "</MessagesDef>"
                                  "</Dico>");
        pointers (ptr, dod); // Reset Dico internal pointers

        assert(dicoId_);
        assert(dicoId_==ptr->id("Dico"));
        idId_         = ptr->id("id");            assert(idId_);
        nameId_       = ptr->id("name");          assert(nameId_);
        typeId_       = ptr->id("type");          assert(typeId_);
        deprecatedId_ = ptr->id("deprecated");    assert(deprecatedId_);
        commentId_    = ptr->id("comment");       assert(commentId_);
        requiredId_   = ptr->id("required");      assert(requiredId_);
        defaultDId_   = ptr->id("defaultD");      assert(defaultDId_);
        defaultSId_   = ptr->id("defaultS");      assert(defaultSId_);
        versionId_    = ptr->id("version");       assert(versionId_);
        itemId_       = ptr->id("Item");          assert(itemId_);
        fieldId_      = ptr->id("Field");         assert(fieldId_);
    }

    return dod;
}

//--------------------------------------------------------------------------
Message Serializer::dicoToMsg () const
{
    const SharedDico& dod = dicoOfDico();
    Message msg = Message (dod, dicoId_);

    if (dico())
    {
        // Version
        if (dico()->version())
        {
            msg.set(versionId_, dico()->version());
        }

        // Fields and Messages
        for (Dico::IdByName::const_iterator it = dico()->names().begin(); it != dico()->names().end(); ++it)
        {
            Id  id = it->second;
            if (id == 0)
            {
                continue; //avoid id=0 name="" type=EMPTY
            }

            const Field& f = dico()->field(id);

            Variant::SeqMessage &fields = msg.get(fieldId_).toSeqMessage();
            fields.push_back( Message(dod,fieldId_) );
            Message &field = fields.back();

            field.set (idId_,         int(id));
            field.set (typeId_,       int(f.type()));
            field.set (nameId_,       f.name());
            field.set (deprecatedId_, f.deprecated());
            field.set (commentId_,    f.comment());

            for (ItemsById::const_iterator jt = f.items().begin(); jt != f.items().end(); ++jt)
            {
                assert( field.get(typeId_).toLong() == Type::MESSAGE );
                Variant::SeqMessage &items = field.get(itemId_).toSeqMessage();
                items.push_back( Message(dod,itemId_) );
                Message &item = items.back();

                item.set (idId_,   int(jt->first));
                item.set (typeId_, int(jt->second.type()));
                item.set (requiredId_, jt->second.required());

                // do not set value if it is zero
                // because by default the value is already zero
                // => this reduce size of the serialized message
                const Variant& v = jt->second.defaultValue();
                switch (jt->second.type())
                {
                case Type::BOOL:   if ( v.toBool())           item.set (defaultDId_, double(v.toBool()) ); break;
                case Type::LONG:   if ( v.toLong())           item.set (defaultDId_, double(v.toLong()) ); break;
                case Type::DOUBLE: if ( v.toDouble())         item.set (defaultDId_,        v.toDouble()); break;
                case Type::STRING: if (!v.toString().empty()) item.set (defaultSId_,        v.toString()); break;

                default:
                    assert(0&&"Expect only Type BOOL LONG DOUBLE STRING. Another type occured.");
                    BOOST_FALLTHROUGH; // for optimization continue with next case
                case Type::EMPTY:
                case Type::MESSAGE:
                case Type::SEQ_BOOL:
                case Type::SEQ_LONG:
                case Type::SEQ_DOUBLE:
                case Type::SEQ_STRING:
                case Type::SEQ_MESSAGE:  // FIXME (olibre): Process this case
                    break;
                }
            }
        }
    }

    return msg;
}

//--------------------------------------------------------------------------
// used by msgToDico()
void Serializer::addField (const Variant::SeqMessage::const_iterator it, Dico* ptr)
{
    const std::string& name = it->get(nameId_).toString();
    signed signedId         = it->get(idId_)  .toLong();
    if (signedId <= ID_MIN || signedId >= ID_MAX)
    {
        std::ostringstream oss;
        oss <<"Field id="<< signedId <<" must be in range ]"<< ID_MIN <<','<< ID_MAX <<"[ in Serializer::msgToDico()";
        const std::string& str = oss.str();
        throw DicoException (str);
    }
    Id id = signedId;

    Field& field = ptr->addField (id, name);

    field.deprecated (it->get(deprecatedId_).toBool());
    field.comment    (it->get(commentId_)   .toString());
    field.type       (it->get(typeId_)      .toLong());
    if (!field.type().check() || field.type().repeated())
    {
        std::ostringstream oss;
        oss <<"Field id="<< signedId <<" has wrong type="<< field.type().str() <<" (must be " ALL_BASIC_TYPES ") in Serializer::msgToDico()";
        const std::string& str = oss.str();
        throw DicoException (str);
    }

    if (! it->empty(itemId_))
    {
        assert(it->type() == Type::MESSAGE);
        if ( field.type() != Type::MESSAGE)
        {
            std::ostringstream oss;
            oss <<"Field name='"<< field.name() <<"' id="<< id <<" must not be composed of items because it is not a message (type="<< field.type().str() <<") in Serializer::msgToDico()";
            const std::string& str = oss.str();
            throw DicoException (str);
        }

        const Variant::SeqMessage& items = it->get(itemId_).toSeqMessage();
        for (Variant::SeqMessage::const_iterator jt = items.begin(); jt != items.end(); ++jt)
        {
            addItem (jt, field);
        }
    }
}

//--------------------------------------------------------------------------
// used by addField() which is used by msgToDico()
void Serializer::addItem (const Variant::SeqMessage::const_iterator jt, Field& field)
{
    bool            required = jt->get(requiredId_).toBool();
    Variant::long_t signedId = jt->get(idId_).toLong();
    Variant::long_t typeLong = jt->get(typeId_).toLong();

    //  assert( typeLong >= Type::EMPTY       );
    //  assert( typeLong <= Type::SEQ_MESSAGE );
    Type type(typeLong);
    if (!type || !type.check())
    {
        std::ostringstream oss;
        oss <<"Item id="<< signedId <<" in message '" + field.name() + "' has wrong type="<< type.str() <<" in Serializer::msgToDico()";
        const std::string& str = oss.str();
        throw DicoException (str);
    }

    if (signedId <= ID_MIN || signedId >= ID_MAX)
    {
        std::ostringstream oss;
        oss <<"Bad item id="<< signedId <<" in message '" + field.name() + "' should be in range ]"<< ID_MIN <<','<< ID_MAX <<"[ in Serializer::msgToDico()";
        const std::string& str = oss.str();
        throw DicoException (str);
    }
    Id id = signedId;

    Item* item = field.add (id, required);
    if (! item)
    {
        std::ostringstream oss;
        oss <<"Duplicated item id="<< id <<" within message '"<< field.name() <<"' in Serializer::msgToDico()";
        const std::string& str = oss.str();
        throw DicoException (str);
    }

    // set default value
    switch (type)
    {
    case Type::BOOL:   item->defaultValue() = bool(           jt->get(defaultDId_).toDouble()); break;
    case Type::LONG:   item->defaultValue() = Variant::long_t(jt->get(defaultDId_).toDouble()); break;
    case Type::DOUBLE: item->defaultValue() = Variant (type,  jt->get(defaultDId_).toDouble()); break;
    case Type::STRING: item->defaultValue() =                 jt->get(defaultSId_).toString() ; break;

    case Type::EMPTY:
        assert(0 && "Unexpected Type::EMPTY");
        BOOST_FALLTHROUGH; // for optimization continue with next case
    default:
        assert(0 && "Unexpected type value");
        BOOST_FALLTHROUGH; // for optimization continue with next case

    case Type::MESSAGE:
    case Type::SEQ_BOOL:
    case Type::SEQ_LONG:
    case Type::SEQ_DOUBLE:
    case Type::SEQ_STRING:
    case Type::SEQ_MESSAGE:
        item->defaultValue() = Variant(type);
        break;
    }
    assert (type == item->type());
    assert (type == field.item(id).type());
}

// ------------------------------------------------------------------------
// Initializes internal Dico pointers
void Serializer::pointers (Dico* ptr, const SharedDico& dic)
{
    uint_fast64_t cs64 = 0;

    for (Dico::IdByName::const_iterator it = ptr->names().begin(); it != ptr->names().end(); ++it)
    {
        Id  id = it->second;
        if (id == 0)
        {
            continue; //avoid id=0 name="" type=EMPTY
        }

        Field& m = ptr->fields()[id];
        assert( m.type().check() );
        assert( m.type() != Type::EMPTY );
        assert( m.type() == Type::MESSAGE || m.items().empty() );
        assert(!m.name().empty());

        // compute checksum on 'id', 'type' and 'name' ('comment' and 'deprecated' do not impact computation)
        cs64 ^= m.type() << (id % 61);
        for (size_t pos=0; pos<m.name().size(); ++pos)
        {
            cs64 ^= m.name()[pos] << ((id+pos) % 59);
        }

        if (m.type() == Type::MESSAGE)
        {
            for (Field::ItemsById::iterator jt = m.items().begin(); jt != m.items().end(); ++jt)
            {
                Id itemId = jt->first;
                const Field& f = ptr->field(itemId);

                // check consistency
                assert( f.type() != Type::EMPTY );
                assert( f.type() == jt->second.type().basic() );
                assert(!f.name().empty());

                // compute checksum on 'id', 'itemId' and 'repeated' (do not impact computation: 'required' and 'default value')
                cs64 ^= f.type() << ((id+itemId) % 60);

                // direct access to field properties from item
                jt->second.ptr(&f);

                // set default Message (cannot be done in DicoXMLReader.cpp)
                if (jt->second.type() == Type::MESSAGE)
                {
                    jt->second.defaultValue() = Message (dic, itemId); // TODO(Oliver): add assert: 'dic' should share 'ptr'
                }
            }
        }
    }

    // compute final checksum in 16 bits
    uint_fast16_t cs16 = (cs64 << 0) ^ (cs64 << 16) ^ (cs64 << 32) ^ (cs64 << 48);
    ptr->checksum(cs16);
    //std::cout <<"Checksum: size="<< ptr->fields().size() <<" cs64="<< cs64 <<" cs16="<< cs16 <<'\n';

    //TODO(Oliver): resize vector/maps => to minimize memory print
}

}  // namespace pbart
