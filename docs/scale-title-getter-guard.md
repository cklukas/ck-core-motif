---
nav_exclude: true
---

# Scale title getter guard

The Scale synthetic title getter now validates the widget class and output
slot before reading title state or querying the label child. Invalid callback
inputs return a deterministic null value; valid title and child-label reads
retain their existing behavior.

The focused source-contract check is
`python3 tests/automated/check-scale-title-getter-guard.py`.
