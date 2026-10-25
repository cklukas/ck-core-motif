---
nav_exclude: true
---
# TextField portability warnings

TextField passed a null pointer through an integer parameter cast when
registering its input method, and initialized a wide-character value by
casting a pointer constant to `wchar_t`.  Those expressions are diagnostic-
prone on 64-bit builds even though they produced zero on common targets.

The calls now pass integer zero to the unsigned `XmImRegister` parameter and
wide-character zero to the `wchar_t` storage.  Runtime behavior and the public
ABI are unchanged, while strict 64-bit compiler diagnostics are removed.

`tests/automated/check-textfield-portability-warnings.py` protects these
type-correct constants.
