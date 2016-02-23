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

#ifndef JNI_CPP_JAVA_CONVERTION_HELPERS_H_
#define JNI_CPP_JAVA_CONVERTION_HELPERS_H_

#include <jni.h>  // JNIEnv jstring

namespace pbartjni
{

// ---------------------------------------------------------------------------
// Convert from Java (jstring) to C (char*) and then to C++ (std::string)
std::string convert (JNIEnv *env, jstring jstr);

// ---------------------------------------------------------------------------
// Convert C++ exception to Java exception
void rethrow_cpp_exception_as_java_exception (JNIEnv *env);

} // namespace pbartjni


#endif // JNI_CPP_JAVA_CONVERTION_HELPERS_H_

