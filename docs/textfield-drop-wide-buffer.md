---
nav_exclude: true
---
# TextField drop wide-character buffers

TextField drop conversion now rejects invalid character counts before wide
buffer sizing and fails cleanly when the wide-character buffer cannot be
allocated.  Pending transfer ownership and the insertion-point repaint are
restored on those failure paths.

Regression coverage: `tests/automated/check-textfield-drop-wide-buffer.py`.
