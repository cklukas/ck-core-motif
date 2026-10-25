---
nav_exclude: true
---
# XmString component insertion failure

`XmStringComponentCreate` now validates its final non-optimized segment
insertion. Allocation failure releases the partial compound string and returns
`NULL` instead of returning an empty or malformed result.

The regression check is
`tests/automated/check-xmstring-component-insertion.py`.
