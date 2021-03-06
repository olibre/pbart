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

#ifndef PBART_EXPORT_H_
#define PBART_EXPORT_H_

#ifdef PBART_EXPORTS
#  warning PBART_EXPORTS already defined: '#PBART_EXPORTS'
#endif

#ifdef PBART_EXPORT
#  warning PBART_EXPORT already defined: '#PBART_EXPORT'
#endif

#if defined(WIN32) || defined(WIN64)
#  ifdef COMPILING_PBART
#    define PBART_EXPORTS __declspec(dllexport)
#  else
#    define PBART_EXPORTS __declspec(dllimport)
#  endif
#else
#  define PBART_EXPORTS
#endif

#endif  // PBART_EXPORT_H_
