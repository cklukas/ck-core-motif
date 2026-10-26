---
nav_exclude: true
---

# TextField replacement growth bounds

TextField replacement buffers now reject capacities that would overflow the
doubling calculation or the wide-character byte conversion before calling
Xt.  This preserves the current text and returns failure rather than wrapping
an allocation size.

`tests/automated/check-textfield-replacement-growth-bounds.py` protects both
normal and preedit replacement paths.
