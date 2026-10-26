---
nav_exclude: true
---
# XmString unparse length bounds

XmString unparse conversion keeps its public `int` length contract, but the
locale conversion routines return `size_t`. The implementation now checks
conversion failure and representability before adding byte counts to the
output length, preventing truncation or signed overflow for very large
compound strings.

`tests/automated/check-xmstring-unparse-length.py` is the focused source
regression for the conversion totals.
