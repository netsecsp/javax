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
