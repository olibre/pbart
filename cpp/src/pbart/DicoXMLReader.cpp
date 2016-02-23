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

#include <string>
#include <sstream>
#include <cassert>

#include <tinyxml2.h>

#include "pbart/DicoXMLReader.h"


#define DICO_ELT         "Dico"

#define FIELDS_DEF_ELT   "FieldsDef"
#define FIELD_DEF_ELT    "FieldDef"
#define MSG_FIELD_ELT    "Field"

#define MESSAGES_DEF_ELT "MessagesDef"
#define MESSAGE_DEF_ELT  "MessageDef"
#define MSG_MESSAGE_ELT  "Message"


namespace pbart
{
  // --------------------------------------------------------------------------
  // parse string
  void DicoXMLReader::parse (const char* xml)
  {
    tinyxml2::XMLDocument doc;
    doc.Parse (xml);
    if (doc.Error())
    {
      std::ostringstream oss;
      oss <<"Loading error #"<< doc.ErrorID() <<" when trying to parse xml string (length = "<< strlen(xml) <<')';
      if (doc.GetErrorStr1()) oss <<" diagnostic1: "<< doc.GetErrorStr1();
      if (doc.GetErrorStr2()) oss <<" diagnostic2: "<< doc.GetErrorStr2();
      const std::string& str = oss.str();
      throw DicoException (str);
    }

    process (doc);
  }

  // --------------------------------------------------------------------------
  // open file
  void DicoXMLReader::load (const char* filename)
  {
    tinyxml2::XMLDocument doc;
    doc.LoadFile (filename);
    if (doc.Error())
    {
      std::ostringstream oss;
      oss <<"Loading error #"<< doc.ErrorID() <<" when trying to parse file '"<< filename <<'\'';
      if (doc.GetErrorStr1()) oss <<" diagnostic1: "<< doc.GetErrorStr1();
      if (doc.GetErrorStr2()) oss <<" diagnostic2: "<< doc.GetErrorStr2();
      const std::string& str = oss.str();
      throw DicoException (str);
    }

    filename_ = filename;
    process (doc);
    filename_ = 0;
  }

  //--------------------------------------------------------------------------
  void DicoXMLReader::process (tinyxml2::XMLDocument& doc)
  {
    assert (doc.Error() == false);

    for (tinyxml2::XMLElement *elem = doc.FirstChildElement(DICO_ELT);
                         elem; elem = elem->NextSiblingElement(DICO_ELT))
    {
        parseDicoHeader   (elem);
        parseDicoFields   (elem);
        parseDicoMessages (elem);
    }

    if (dico_.names().size() <= 1)
    {
      std::string str("Dictionary is empty");
      if (filename_)
      {
         str += " => Verify file '";
         str += filename_;
         str += '\'';
      }
      throw DicoException (str);
    }
  }

  //--------------------------------------------------------------------------
  void DicoXMLReader::parseDicoHeader (tinyxml2::XMLElement *elem)
  {
    int v = getIntegerAttribute (elem, "version");
    dico_.version (v);
  }

  //--------------------------------------------------------------------------
  void DicoXMLReader::parseDicoFields (tinyxml2::XMLElement *elem)
  {
    for (tinyxml2::XMLElement *child = elem->FirstChildElement(FIELDS_DEF_ELT);
                        child; child = child->NextSiblingElement(FIELDS_DEF_ELT))
    {
        parseFieldsDefElt (child);
    }
  }

  //--------------------------------------------------------------------------
  void DicoXMLReader::parseFieldsDefElt (tinyxml2::XMLElement *elem)
  {
    for (tinyxml2::XMLElement *child = elem->FirstChildElement(FIELD_DEF_ELT);
                        child; child = child->NextSiblingElement(FIELD_DEF_ELT))
    {
        parseFieldDefElt (child);
    }
  }

  //--------------------------------------------------------------------------
  void DicoXMLReader::parseDicoMessages (tinyxml2::XMLElement *elem)
  {
    for (tinyxml2::XMLElement *child = elem->FirstChildElement(MESSAGES_DEF_ELT);
                        child; child = child->NextSiblingElement(MESSAGES_DEF_ELT))
    {
        parseMessagesDefElt (child);
    }
  }

  //--------------------------------------------------------------------------
  void DicoXMLReader::parseMessagesDefElt (tinyxml2::XMLElement *elem)
  {
    for (tinyxml2::XMLElement *child = elem->FirstChildElement(MESSAGE_DEF_ELT);
                        child; child = child->NextSiblingElement(MESSAGE_DEF_ELT))
    {
        parseMessageDefElt (child);
    }
  }

  //--------------------------------------------------------------------------
  std::string DicoXMLReader::context (const tinyxml2::XMLElement&) const
  {
    if (filename_)
    {
      std::string str = " in file='";
      str += filename_;
      str += '\'';
      return str;
    }
    else
    {
      return " in XML string";
    }
  }

  //--------------------------------------------------------------------------
  // <FieldDef name="Qty"    id="0" type="long"    deprecated="false" comment="Need Xetra" />
  void DicoXMLReader::parseFieldDefElt (tinyxml2::XMLElement *elem)
  {
    const std::string& name = getStringAttribute  (elem, "name");
    signed signedId         = getIntegerAttribute (elem, "id");

    if (signedId <= ID_MIN)
    {
      std::ostringstream oss;
      oss <<"Field '"<< name <<"' cannot have a null or negative id="<< signedId << context(*elem);
      const std::string& str = oss.str();
      throw DicoException (str);
    }

    Id  id = signedId;
    if (id > ID_MAX)
    {
      std::ostringstream oss;
      oss <<"Field '"<< name <<"' cannot have id="<< id <<" greater than "<< ID_MAX << context(*elem);
      const std::string& str = oss.str();
      throw DicoException (str);
    }

    Field& field = dico_.addField (id, name);
    field.deprecated (      getBooleanAttribute (elem, "deprecated")     );
    field.comment    (      getStringAttribute  (elem, "comment", false) );
    field.type       ( Type(getStringAttribute  (elem, "type"))          );

    switch (field.type())
    {
    case Type::EMPTY:
      {
        std::string str = "Field '";
        str += field.name();
        str += "' use bad type (must be: " ALL_BASIC_TYPES ")";
        str += context(*elem);
        throw DicoException (str);
      }

    case Type::MESSAGE:
      {
        std::string str = "Field '";
        str += field.name();
        str += "' cannot use type MESSAGE (must be: " ALL_BASIC_TYPES ")";
        str += context(*elem);
        throw DicoException (str);
      }
    }
  }

  //--------------------------------------------------------------------------
  // <MessageDef id="0" name="market.StOrder" deprecated="false"> // ajout comment
  void DicoXMLReader::parseMessageDefElt (tinyxml2::XMLElement *elem)
  {
    const std::string& name = getStringAttribute  (elem, "name");
    signed signedId         = getIntegerAttribute (elem, "id");

    if (signedId <= ID_MIN)
    {
      std::ostringstream oss;
      oss <<"Message '"<< name <<"' cannot have a null or negative id="<< signedId << context(*elem);
      const std::string& str = oss.str();
      throw DicoException (str);
    }

    Id id = signedId;
    if (id > ID_MAX)
    {
      std::ostringstream oss;
      oss <<"Message '"<< name <<"' cannot have id="<< id <<" greater than "<< ID_MAX << context(*elem);
      const std::string& str = oss.str();
      throw DicoException (str);
    }

    Field& message = dico_.addField (id, name);
    message.type       ( Type::MESSAGE );
    message.deprecated ( getBooleanAttribute (elem, "deprecated")     );
    message.comment    ( getStringAttribute  (elem, "comment", false) );

    for (tinyxml2::XMLElement *child = elem->FirstChildElement();
                        child; child = child->NextSiblingElement())
    {
      const std::string name = child->Name();
      if (MSG_FIELD_ELT == name || MSG_MESSAGE_ELT == name)
      {
        addItem (child, message); //add item Message
      }
      else
      {
        std::string str = "Unexpected parameter '";
        str += name;
        str += "' (must be "   MSG_FIELD_ELT   " or "   MSG_MESSAGE_ELT   ")";
        str += context(*child);
        throw DicoException (str);
      }
    }
  }

  //--------------------------------------------------------------------------
  //   <Field name="Qty"    required="true"  />
  void DicoXMLReader::addItem (tinyxml2::XMLElement *elem, Field& message)
  {
    // get attributes from XML
    std::string name     = getStringAttribute  (elem, "name");
    bool        required = getBooleanAttribute (elem, "required");  //required is stored within Item::required attribute
    bool        repeated = getBooleanAttribute (elem, "repeated");  //repeated is used to calculmated type
                                                                    //   => type is then stored within Variant (default value)
    // find ID
    Id id = dico_.id (name);
    if (id == 0)
    {
      std::string str;
      str += "Missing definition for '" + name + "' in message '" + message.name() + '\'' + context(*elem);
      throw DicoException(str);
    }

    // compute type depending on 'repeated' value
    Type type = dico_.field(id).type();
    if (repeated)
    {
      type.setSequence();
    }
    assert(type.repeated() == repeated);

    // add it
    Item* item = message.add (id, required);
    if (! item)
    {
      std::ostringstream oss;
      oss <<"Duplicated item '"<< name <<"' (id="<< id <<") within message '"<< message.name() <<'\''<< context(*elem);
      const std::string& str = oss.str();
      throw DicoException (str);
    }

    // set default value
    switch (type)
    {
      case Type::BOOL:   item->defaultValue() =                getBooleanAttribute (elem, "default");   break;
      case Type::LONG:   item->defaultValue() =                getIntegerAttribute (elem, "default");   break;
      case Type::DOUBLE: item->defaultValue() = Variant (type, getDoubleAttribute  (elem, "default"));  break;
      case Type::STRING: item->defaultValue() =                getStringAttribute  (elem, "default");   break;

      default:
        assert(0 && "Unexpected type value");
      case Type::MESSAGE:
      case Type::SEQ_BOOL:
      case Type::SEQ_LONG:
      case Type::SEQ_DOUBLE:
      case Type::SEQ_STRING:
      case Type::SEQ_MESSAGE:
        item->defaultValue() = Variant(type);
        break;
    }
    assert (item->type() == type);
  }

  //--------------------------------------------------------------------------
  const char* DicoXMLReader::getStringAttribute (tinyxml2::XMLElement *elem, const char name[], const bool required)
  {
    assert(elem);

    const char* str = elem->Attribute (name);
    if (str)
    {
      return str;
    }

    if (required)
    {
      std::string str = "Missing attribute '";
      str += name;
      str += "' (the value can be any string)";
      str += context(*elem);
      throw DicoException (str);
    }
    else
    {
      return "";
    }
  }

  //--------------------------------------------------------------------------
  int DicoXMLReader::getIntegerAttribute (tinyxml2::XMLElement *elem, const char name[], const bool required)
  {
    assert(elem);

    int value;
    tinyxml2::XMLError error = elem->QueryIntAttribute (name, &value);

    switch (error)
    {
    case tinyxml2::XML_SUCCESS:
  //case tinyxml2::XML_NO_ERROR:
      return value;

    case tinyxml2::XML_NO_ATTRIBUTE:
      if (required)
      {
        std::string str = "Missing attribute '";
        str += name;
        str += "' with an integer number";
        str += context(*elem);
        throw DicoException (str);
      }
      break;

    case tinyxml2::XML_WRONG_ATTRIBUTE_TYPE:
      {
        std::string str = "Bad type for attribute '";
        str += name;
        str += "' => must be INTEGER";
        str += context(*elem);
        throw DicoException (str);
      }
      break;

    case tinyxml2::XML_ERROR_FILE_NOT_FOUND:
    case tinyxml2::XML_ERROR_FILE_COULD_NOT_BE_OPENED:
    case tinyxml2::XML_ERROR_FILE_READ_ERROR:
    case tinyxml2::XML_ERROR_ELEMENT_MISMATCH:
    case tinyxml2::XML_ERROR_PARSING_ELEMENT:
    case tinyxml2::XML_ERROR_PARSING_ATTRIBUTE:
    case tinyxml2::XML_ERROR_IDENTIFYING_TAG:
    case tinyxml2::XML_ERROR_PARSING_TEXT:
    case tinyxml2::XML_ERROR_PARSING_CDATA:
    case tinyxml2::XML_ERROR_PARSING_COMMENT:
    case tinyxml2::XML_ERROR_PARSING_DECLARATION:
    case tinyxml2::XML_ERROR_PARSING_UNKNOWN:
    case tinyxml2::XML_ERROR_EMPTY_DOCUMENT:
    case tinyxml2::XML_ERROR_MISMATCHED_ELEMENT:
    case tinyxml2::XML_ERROR_PARSING:
    case tinyxml2::XML_CAN_NOT_CONVERT_TEXT:
    case tinyxml2::XML_NO_TEXT_NODE:
    case tinyxml2::XML_ERROR_COUNT:
    default:
      //oss <<"Ignored error="<< status <<" for attribute '"<< name <<'\''<< context(*elem);
      //throw DicoException (oss.str());
      assert(0 && "QueryIntAttribute() returned an error that is ignored (to be fixed if required)");
    }

    return 0;
  }

  //--------------------------------------------------------------------------
  bool DicoXMLReader::getBooleanAttribute (tinyxml2::XMLElement *elem, const char name[], const bool required)
  {
    assert(elem);

    bool value = false;
    tinyxml2::XMLError error = elem->QueryBoolAttribute (name, &value);

    switch (error)
    {
    case tinyxml2::XML_SUCCESS:
  //case tinyxml2::XML_NO_ERROR:
      return value;

    case tinyxml2::XML_NO_ATTRIBUTE:
      if (required)
      {
        std::string str = "Missing attribute '";
        str += name;
        str += "' of type BOOLEAN";
        str += context(*elem);
        throw DicoException (str);
      }
      break;

    case tinyxml2::XML_WRONG_ATTRIBUTE_TYPE:
      {
        std::string str = "Bad value type for attribute '";
        str += name;
        str += "' => must be BOOLEAN (true/false/1/0)";
        str += context(*elem);
        throw DicoException (str);
      }

    case tinyxml2::XML_ERROR_FILE_NOT_FOUND:
    case tinyxml2::XML_ERROR_FILE_COULD_NOT_BE_OPENED:
    case tinyxml2::XML_ERROR_FILE_READ_ERROR:
    case tinyxml2::XML_ERROR_ELEMENT_MISMATCH:
    case tinyxml2::XML_ERROR_PARSING_ELEMENT:
    case tinyxml2::XML_ERROR_PARSING_ATTRIBUTE:
    case tinyxml2::XML_ERROR_IDENTIFYING_TAG:
    case tinyxml2::XML_ERROR_PARSING_TEXT:
    case tinyxml2::XML_ERROR_PARSING_CDATA:
    case tinyxml2::XML_ERROR_PARSING_COMMENT:
    case tinyxml2::XML_ERROR_PARSING_DECLARATION:
    case tinyxml2::XML_ERROR_PARSING_UNKNOWN:
    case tinyxml2::XML_ERROR_EMPTY_DOCUMENT:
    case tinyxml2::XML_ERROR_MISMATCHED_ELEMENT:
    case tinyxml2::XML_ERROR_PARSING:
    case tinyxml2::XML_CAN_NOT_CONVERT_TEXT:
    case tinyxml2::XML_NO_TEXT_NODE:
    case tinyxml2::XML_ERROR_COUNT:
    default:
      //oss <<"Ignored error="<< status <<" for attribute '"<< name <<'\''<< context(*elem);
      //throw DicoException (oss.str());
      assert(0 && "QueryIntAttribute() returned an error that is ignored (to be fixed if required)");
    }

    return 0;
  }

  //--------------------------------------------------------------------------
  double DicoXMLReader::getDoubleAttribute (tinyxml2::XMLElement *elem, const char name[], const bool required)
  {
    assert(elem);

    double value = false;
    tinyxml2::XMLError error = elem->QueryDoubleAttribute (name, &value);

    switch (error)
    {
    case tinyxml2::XML_SUCCESS:
  //case tinyxml2::XML_NO_ERROR:
      return value;

    case tinyxml2::XML_NO_ATTRIBUTE:
      if (required)
      {
        std::string str = "Missing attribute '";
        str += name;
        str += "' of type DOUBLE (floating-point number)";
        str += context(*elem);
        throw DicoException (str);
      }
      break;

    case tinyxml2::XML_WRONG_ATTRIBUTE_TYPE:
      {
        std::string str = "Bad value type for attribute '";
        str += name;
        str += "' => must be DOUBLE (floating-point number)";
        str += context(*elem);
        throw DicoException (str);
      }

    case tinyxml2::XML_ERROR_FILE_NOT_FOUND:
    case tinyxml2::XML_ERROR_FILE_COULD_NOT_BE_OPENED:
    case tinyxml2::XML_ERROR_FILE_READ_ERROR:
    case tinyxml2::XML_ERROR_ELEMENT_MISMATCH:
    case tinyxml2::XML_ERROR_PARSING_ELEMENT:
    case tinyxml2::XML_ERROR_PARSING_ATTRIBUTE:
    case tinyxml2::XML_ERROR_IDENTIFYING_TAG:
    case tinyxml2::XML_ERROR_PARSING_TEXT:
    case tinyxml2::XML_ERROR_PARSING_CDATA:
    case tinyxml2::XML_ERROR_PARSING_COMMENT:
    case tinyxml2::XML_ERROR_PARSING_DECLARATION:
    case tinyxml2::XML_ERROR_PARSING_UNKNOWN:
    case tinyxml2::XML_ERROR_EMPTY_DOCUMENT:
    case tinyxml2::XML_ERROR_MISMATCHED_ELEMENT:
    case tinyxml2::XML_ERROR_PARSING:
    case tinyxml2::XML_CAN_NOT_CONVERT_TEXT:
    case tinyxml2::XML_NO_TEXT_NODE:
    case tinyxml2::XML_ERROR_COUNT:
    default:
      //oss <<"Ignored error="<< status <<" for attribute '"<< name <<'\''<< context(*elem);
      //throw DicoException (oss.str());
      assert(0 && "QueryIntAttribute() returned an error that is ignored (to be fixed if required)");
    }

    return 0;
  }

}  // namespace pbart
