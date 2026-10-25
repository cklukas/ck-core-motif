---
nav_exclude: true
---

# XmIm oversized nested-list guard

`XmIm` builds `XVaNestedList` values from a portable, fixed spelling of ten
name/value pairs.  Resource updates can legitimately accumulate more entries
than that.  The old assertion aborted the application when the count exceeded
ten, even in normal release builds.

`VaCopy` now rejects only that unsupported conversion size and returns `NULL`.
Its existing callers already treat a failed nested-list construction as a
recoverable fallback, so valid smaller lists and the public Motif ABI are
unchanged.  The static regression check is
`tests/automated/check-xmim-oversized-nested-list.py`.
