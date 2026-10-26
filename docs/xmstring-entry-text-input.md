---
nav_exclude: true
---

# XmString entry text input validation

The internal `_XmEntryTextSet` helper now treats a null entry as a no-op and a
null value for an immediate segment as an empty string. Pointer-backed and
unoptimized segments retain their existing ownership behavior. This prevents
the immutable-segment path from passing a null source to `strcpy`.

The focused source check is:

```sh
python3 tests/automated/check-xmstring-entry-text-input.py
```
