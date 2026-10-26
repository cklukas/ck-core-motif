---
nav_exclude: true
---

# TextField replacement reallocation safety

Both normal and preedit TextField replacement paths now grow byte and
wide-character storage through temporary pointers.  Allocation failure returns
`False` before the field publishes a lost or NULL buffer; successful growth
publishes the new capacity and pointer together.

`tests/automated/check-textfield-replacement-realloc.py` protects both
replacement paths.
