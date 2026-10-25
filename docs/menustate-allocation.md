---
nav_exclude: true
---
# MenuState allocation

Menu-state creation now returns `NULL` before publishing or initializing a
failed state record. Drag-mode updates also tolerate that failure, preventing
menu interaction from dereferencing a null state object.

`tests/automated/check-menustate-allocation.py` protects this contract.
