---
nav_exclude: true
---
# TextField clear-selection allocation

Clearing a multibyte TextField selection now checks its temporary wide-space
buffer before filling it.  Allocation failure repaints the insertion point and
returns without dereferencing a null buffer.

Regression coverage: `tests/automated/check-textfield-clear-selection.py`.
