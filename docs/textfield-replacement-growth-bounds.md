---
nav_exclude: true
---

# TextField replacement growth bounds

TextField replacement buffers now use one checked growth helper that rejects
signed-length overflow, doubling overflow, and wide-character byte conversion
overflow before calling Xt. This preserves the current text and returns
failure rather than wrapping an allocation size.

`tests/automated/check-textfield-replacement-growth-bounds.py` protects both
normal and preedit replacement paths.
