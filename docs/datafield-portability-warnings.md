---
nav_exclude: true
---
# DataField portability warnings

DataField passed a null pointer through an integer parameter cast when
registering its input method, and initialized a wide-character value by
casting a pointer constant to `wchar_t`.  Those expressions are undefined or
diagnostic-prone on 64-bit builds even though they happened to produce zero on
common targets.

The calls now pass integer zero to the unsigned `XmImRegister` parameter and
wide-character zero to the `wchar_t` storage.  Runtime behavior and the public
ABI are unchanged, while ARM64 and other 64-bit compilers no longer need to
diagnose these paths.

`tests/automated/check-datafield-portability-warnings.py` protects the
type-correct constants.
