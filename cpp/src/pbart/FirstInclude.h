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

#ifdef PBART_FIRST_INCLUDE_H_
#error "This file must only be used by C++ pbart files, must be the first included file and must be included only once"
#endif

#ifndef PBART_FIRST_INCLUDE_H_
#define PBART_FIRST_INCLUDE_H_

#ifdef PBART_EXPORTS
#  warning PBART_EXPORTS already defined: '#PBART_EXPORTS'
#endif

#ifdef PBART_EXPORT
#  warning PBART_EXPORT already defined: '#PBART_EXPORT'
#endif

#define COMPILING_PBART 1

#endif  // PBART_FIRST_INCLUDE_H_
