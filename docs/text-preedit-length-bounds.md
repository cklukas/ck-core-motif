---
nav_exclude: true
---

# XIM preedit length bounds

`XIMPreeditDrawCallbackStruct.text->length` is an `int`.  The text widget
previously copied it into an `unsigned short` before checking the 512-byte
preedit limit, allowing large values to wrap and bypass that limit.  The
callback now retains the source width, rejects negative lengths, and applies
the existing limit before using the value in geometry or allocation arithmetic.

`tests/automated/check-text-preedit-length-width.py` protects this callback
contract.
