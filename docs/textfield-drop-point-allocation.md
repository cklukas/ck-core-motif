---
nav_exclude: true
---
# TextField drag/drop point allocation

TextField drag/drop setup now checks the optional XPoint allocation before
writing event coordinates. If allocation fails, the transfer sink receives
the existing null location pointer instead of triggering a null dereference.

Regression coverage: tests/automated/check-textfield-drop-point-allocation.py.
