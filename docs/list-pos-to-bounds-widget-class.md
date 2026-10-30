---
nav_exclude: true
---
# List position-bounds widget guard

`XmListPosToBounds` now rejects non-List widgets before realization checks or
list-private geometry access. Valid list bounds queries are unchanged, while
malformed callers no longer reinterpret unrelated widget memory as list state.

The source contract is checked by
`tests/automated/check-list-pos-to-bounds-widget-class.py`.
