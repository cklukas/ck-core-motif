---
nav_exclude: true
---

# XmString-table conversion rejects invalid multibyte input

The shared `GetNextXmString` parser now reports an invalid locale multibyte
sequence as a failed component instead of returning a truncated component to
`CvtStringToXmStringTable`. Its temporary component is freed and set to `NULL`,
so the existing table-conversion rollback path releases earlier components and
does not publish partial Unicode data.

The focused source-contract check is
`tests/automated/check-string-table-converter-allocation.py`.
