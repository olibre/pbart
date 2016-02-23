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

#ifndef PBART_DICOXMLREADER_H_
#define PBART_DICOXMLREADER_H_


#include <string>

#include "pbart/Message.h"

namespace tinyxml2
{
  class XMLElement;
  class XMLDocument;
}

namespace pbart
{
  class DicoXMLReader
  {
    public:
      explicit DicoXMLReader (Dico& d) : dico_(d), filename_(0) { }

      void parse (const char* xmlString);
      void load  (const char* filename);

    private:
      void process (tinyxml2::XMLDocument& doc);

      std::string context(const tinyxml2::XMLElement& elem) const;
      void parseDicoHeader     (tinyxml2::XMLElement* elem);
      void parseDicoFields     (tinyxml2::XMLElement* elem);
      void parseDicoMessages   (tinyxml2::XMLElement* elem);
      void parseFieldsDefElt   (tinyxml2::XMLElement* elem);
      void parseMessagesDefElt (tinyxml2::XMLElement* elem);
      void parseFieldDefElt    (tinyxml2::XMLElement* elem);
      void parseMessageDefElt  (tinyxml2::XMLElement* elem);
      void addItem             (tinyxml2::XMLElement* elem, Field& message);

      const char* getStringAttribute  (tinyxml2::XMLElement* elem, const char name[], const bool required = true);
      int         getIntegerAttribute (tinyxml2::XMLElement* elem, const char name[], const bool required = true);
      bool        getBooleanAttribute (tinyxml2::XMLElement* elem, const char name[], const bool required = true);
      double      getDoubleAttribute  (tinyxml2::XMLElement *elem, const char name[], const bool required = true);

    protected:
      Dico& dico_;
      const char* filename_;
  };

}  // namespace pbart

#endif  // PBART_DICOXMLREADER_H_
