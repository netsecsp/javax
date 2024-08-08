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

import java.util.concurrent.ConcurrentHashMap;

public class CInstanceManager {

    public interface IAsynMessageEvents {
        int onMessage(int message, long lparam1, long lparam2, CUnknown object);
    }

    private static ConcurrentHashMap<String, IAsynMessageEvents> mMapEvents = new ConcurrentHashMap<>();

    //@CalledByNative/java
    public static IAsynMessageEvents get(String name) {
        return mMapEvents.get(name);
    }

    public static void add(String name, IAsynMessageEvents events) {
        if (events != null) {
            mMapEvents.put(name, events);
        }
    }

    public static void remove(String name) {
        mMapEvents.remove(name);
    }

    //@CalledByNative
    private native static long   getNativeObject();

    //@Called in main, type=1: app会自动通知java虚拟机退出
    public  native static void   setMainType(int type);
    public  native static String getLibraryPath();
    public  native static int    invoke(Object thread, Object events, int message, long lparam1, long lparam2, CUnknown object);
}
