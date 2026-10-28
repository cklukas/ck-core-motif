---
nav_exclude: true
---

# XPM color-table input guard

XPM serialization now rejects a nonzero color count paired with a null color
table before iterating the table.  This keeps malformed caller input on the
normal `XpmFileInvalid` path instead of dereferencing a null pointer.

Regression: `tests/automated/check-xpm-color-table-input.py`
