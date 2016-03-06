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
#include "pbart/Types.h"

#include <string>
#include <sstream>
#include <algorithm> //std::transform


namespace pbart
{

//----------------------------------------------------------------------------
Type::Enum Type::parse (const std::string& str)
{
    std::string upper = str;
    std::transform (upper.begin(), upper.end(), upper.begin(), ::toupper);

    if (upper == "BOOL")         return BOOL;
    if (upper == "LONG")         return LONG;
    if (upper == "DOUBLE")       return DOUBLE;
    if (upper == "STRING")       return STRING;
    if (upper == "MESSAGE")      return MESSAGE;

    //  if (upper == "SEQ_BOOL")     return SEQ_BOOL;
    //  if (upper == "SEQ_LONG")     return SEQ_LONG;
    //  if (upper == "SEQ_DOUBLE")   return SEQ_DOUBLE;
    //  if (upper == "SEQ_STRING")   return SEQ_STRING;
    //  if (upper == "SEQ_MESSAGE")  return SEQ_MESSAGE;

    // std::ostringstream oss;
    // oss <<"Unexpected type='"<< stringType <<"' (must be: " ALL_BASIC_TYPES ")";
    // throw DicoException (oss.str());
    return EMPTY;
}

} // namespace pbart
