---
nav_exclude: true
---
# DataField multibyte counting progress

DataField counts characters by repeatedly calling `mblen`.  A zero return
means that no multibyte character was consumed (for example, an embedded NUL
or an incomplete sequence).  The old loop only stopped on negative errors, so
the zero result left both the pointer and byte count unchanged and could hang
the event thread.

The loop now stops for every non-positive result.  Valid multibyte sequences
continue through the same path, while malformed input is rejected promptly.
The source regression in `tests/automated/check-datafield-mblen-progress.py`
protects the termination condition.
