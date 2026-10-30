---
nav_exclude: true
---
# Label conversion allocation guards

Label and LabelGadget drag conversion now checks target-list, pixmap,
background, and foreground result allocations. Allocation failure reports
`XmCONVERT_REFUSE` before any result is dereferenced or published.

Locale conversion also measures the caller's source string before invoking the
converter; it never reads the result pointer before that pointer is populated.

The focused regression is
`tests/automated/check-label-conversion-allocation.py`, with the source-length
contract covered by `tests/automated/check-label-locale-length-source.py`.
