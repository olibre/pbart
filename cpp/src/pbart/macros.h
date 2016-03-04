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

#ifndef PBART_DEBUG_H_
#define PBART_DEBUG_H_

#ifdef PBART_ENABLE_TRACE
#  ifdef _MSC_VER
#    include <windows.h>
#    include <sstream>
#    define PBART_TRACE(x)              \
do {  std::stringstream s;  s << x;     \
    OutputDebugString(s.str().c_str()); \
} while (0)
#  else
#    include <iostream>
#    define PBART_TRACE(x)  do { std::clog << x; }while(0)
#  endif
#else
#  define PBART_TRACE(x)
#endif


// BOOST_FALLTHROUGH has replaced PBART_FALLTHROUGH
//#ifdef __clang__
//#  define PBART_FALLTHROUGH [[clang::fallthrough]];
//#else
//#  define PBART_FALLTHROUGH
//#endif


#endif  // PBART_DEBUG_H_
