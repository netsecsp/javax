package com.frame.api;

public class CUnknown {

    public interface IUnknownFactory {
        CUnknown create(String name, long nativeObject, boolean refCounted);
    }

    private static IUnknownFactory unknownFactory;
    private long    nativeObject;
    private boolean needReleased;

    protected CUnknown(long nativeObject, boolean needReleased) {
        this.nativeObject = nativeObject;
        this.needReleased = needReleased;
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();
        release();
    }

    //@CalledByNative
    private long getNativeObject() {
        return nativeObject;
    }

    private native static void nativeRelease(long nativeObject);
    public void release() {
        if (nativeObject!= 0) {
            if (needReleased) {
                nativeRelease(nativeObject);
            }
            nativeObject = 0;
        }
    }

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
