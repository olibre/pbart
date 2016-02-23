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

#ifndef PBART_COMMONEXCEPTION_H_
#define PBART_COMMONEXCEPTION_H_

// Debug
#ifdef ENABLE_TRACE
#  ifdef _MSC_VER
#    include <windows.h>
#    include <sstream>
#    define TRACE(x)                            \
      do {  std::stringstream s;  s << x;       \
            OutputDebugString(s.str().c_str()); \
        } while (0)
#  else
#    include <iostream>  //NOLINT Streams are highly discouraged.
#    define TRACE(x)  do { std::clog << x; }while(0)
#  endif
#else
#  define TRACE(x)
#endif

#include <string>
#include <stdexcept>


namespace common
{
  class Exception : public std::logic_error
  {
    public:
      //notused  Exception()                         : std::logic_error("pbart exception") { }
      explicit   Exception (const std::string& what) : std::logic_error(what)            { }
  };
}


//---------------------------------------------------
// Generic operator <<
template <typename T, typename MSG>
T& operator << (T& os, const common::Exception& e)
{
  return os << e.what();
}


#endif  // PBART_COMMONEXCEPTION_H_

