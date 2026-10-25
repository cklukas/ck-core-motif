---
nav_exclude: true
---
# XmString ASN.1 truncation overflow

`_XmStringTruncateASN1` now checks the accumulated component size before
adding it to the 16-bit legacy length accumulator. An oversized compound
string therefore fails cleanly instead of wrapping `used` and resizing to an
undersized buffer. Existing valid encodings and truncation behavior are
unchanged.

The regression check is
`tests/automated/check-xmstring-truncate-overflow.py`.
