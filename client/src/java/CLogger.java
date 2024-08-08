/******************************************************
 Copyright (c) netsecsp 2012-2032, All rights reserved.
 
 Developer: Shengqian Yang, from China, E-mail: netsecsp@hotmail.com, last updated 01/15/2024
 http://asynframe.sf.net
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are
 met:
 
 * Redistributions of source code must retain the above
 copyright notice, this list of conditions and the
 following disclaimer.
 
 * Redistributions in binary form must reproduce the
 above copyright notice, this list of conditions
 and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************/
package com.frame.api;

public class CLogger {

    public static final int LL_VERBOSE = 1;
    public static final int LL_DEBUG   = 2;
    public static final int LL_INFO    = 3;
    public static final int LL_WARN    = 4;
    public static final int LL_ERROR   = 5;

    public static void v(String text) {
        nativeWrite(LL_VERBOSE, text, null, 0);
    }
    public static void d(String text) {
        nativeWrite(LL_DEBUG  , text, null, 0);
    }
    public static void i(String text) {
        nativeWrite(LL_INFO   , text, null, 0);
    }
    public static void w(String text) {
        nativeWrite(LL_WARN   , text, null, 0);
    }
    public static void e(String text) {
        nativeWrite(LL_ERROR  , text, null, 0);
    }

    public static void write(int level, String text) {
        nativeWrite(level, text, Thread.currentThread().getStackTrace()[2].getClassName() + "." + Thread.currentThread().getStackTrace()[2].getMethodName(), Thread.currentThread().getStackTrace()[2].getLineNumber());
    }

    private native static void nativeWrite(int level, String text, String file, int line);
}