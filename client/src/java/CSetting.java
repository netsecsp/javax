package com.frame.api;

public class CSetting {
    public native static String getString(String name, String defaultvalue);
    public native static void   setString(String name, String value);

    public native static int    getNumber(String name, int defaultvalue);
    public native static void   setNumber(String name, int value);

    public native static void   save();
}