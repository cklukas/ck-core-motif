# XmString ASN.1 truncation header bounds

`_XmStringTruncateASN1` now verifies that each remaining component contains a
complete tag/length header before reading its encoded length. Both short and
long ASN.1 length forms are checked, so truncated external compound text is
rejected without reading past the supplied buffer.

The focused source-contract check is:

```text
python3 tests/automated/check-xmstring-truncate-header-bounds.py
```
