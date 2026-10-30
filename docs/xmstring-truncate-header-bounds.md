# XmString ASN.1 truncation header bounds

`_XmStringTruncateASN1` now verifies that the top-level header and each
remaining component contain complete tag/length headers before reading encoded
lengths. Both short and long ASN.1 length forms are checked, so truncated
external compound text is rejected without reading past the supplied buffer.

The focused source-contract check is:

```text
python3 tests/automated/check-xmstring-truncate-header-bounds.py
```
