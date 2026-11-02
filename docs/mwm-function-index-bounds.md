---
nav_exclude: true
---

# MWM function-table index bounds

The WSM-facing MWM helpers that locate `f.exec`, `f.action`, and `f.nop` in
the built-in function table now reject null result pointers and clamp a
caller-provided table length to the actual table.  A negative, zero, or
oversized length now produces the safe default index instead of leaving an
output undefined or reading beyond the static table.

Valid callers that pass the built-in table length retain the same indices. The
focused source-level regression is
`tests/automated/check-mwm-function-index-bounds.py`.
