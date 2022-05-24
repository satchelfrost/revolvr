package com.imminenteminence.fts;

// BOOYAAHH
public class MainActivity extends android.app.NativeActivity {
  static {
    System.loadLibrary("openxr_loader");
    System.loadLibrary("fts");
  }
}
