---
nav_exclude: true
---

# XmString byte-compare length width

`XmCvtXmStringToByteStream` reports an `unsigned int` byte count.  The legacy
`XmStringByteCompare` helper previously stored those counts in `unsigned
short`, truncating compound strings larger than 65,535 bytes before comparing
them.  The local lengths now retain the converter's full width, so differences
after the old 16-bit boundary are not silently ignored.

`tests/automated/check-xmstring-byte-compare-width.py` protects this API
contract.
