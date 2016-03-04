/*
 *  Copyright (c) 2014-2016 olibre (olibre@Lmap.org)
 *
 *  This file is part of the program Pbart - Protocol Buffers at Runtime.
 *
 *  Pbart is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License (AGPL)
 *  version 3 or (at your option) any later version.
 * 
 *  You should have received a copy of the GNU Affero General Public License 
 *  along with Pbart (maybe in file LICENSE).
 *  If not, see <http://www.gnu.org/licenses/>.
 */
package org.Lmap.pbart;

/**
 * @author olibre (olibre@Lmap.org)
 *
 */
public class Exception extends java.lang.Exception {

    public Exception(String text) {
        super(text);
    }

    public Exception(Throwable exception) {
        super(exception);
    }

    public Exception(String text, Throwable exception) {
        super(text, exception);
    }
}
