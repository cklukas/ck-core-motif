---
nav_exclude: true
---
# TextStrSo wide callback conversion

Wide modify-verify callbacks may replace both their text pointer and length.
The source now copies that callback text into bounded, owned storage before
adding a terminator and converting it, avoiding writes into callback-owned
buffers.

Regression coverage: `tests/automated/check-textstrso-wide-callback-copy.py`.
