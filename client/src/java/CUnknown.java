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

import java.io.Closeable; //通过CUnknown继承Closeable接口显式管理释放对应native资源
import java.io.IOException;

public class CUnknown implements Closeable {

    public interface IUnknownFactory {
        CUnknown create(String name, long nativeObject, boolean refCounted);
    }

    private static  IUnknownFactory unknownFactory;
    private long    nativeObject;
    private boolean needReleased;

    protected CUnknown(long nativeObject, boolean needReleased) {
        this.nativeObject = nativeObject;
        this.needReleased = needReleased;
    }

    @Override
    public void close() throws IOException {
        if (nativeObject!= 0) {
            if (needReleased) {
                nativeRelease(nativeObject);
            }
            nativeObject = 0;
        }
    }

    //@CalledByNative/java
    public long getNativeObject() {
        return nativeObject;
    }

    private native static void nativeRelease(long nativeObject);

    //@CalledByNative
    private static CUnknown create(String name, long nativeObject, boolean needReleased) {
        if (name != null && unknownFactory != null) {
            CUnknown object = unknownFactory.create(name, nativeObject, needReleased);
            if (object != null) {
                return object;
            }
        }
        return new CUnknown(nativeObject, needReleased);
    }

    public static void setUnknownFactory(IUnknownFactory factory) {
        unknownFactory = factory;
    }
}
