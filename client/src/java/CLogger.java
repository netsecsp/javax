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