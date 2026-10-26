---
nav_exclude: true
---

# XIM preedit position bounds

The XIM preedit callback supplies caret, change-start, and change-length
positions as signed protocol fields.  Negative values are malformed and could
otherwise be added to `XmTextPosition` state before the normal range clamp.
`PreeditDraw` now rejects all three before highlighting or updating the text
source, preserving the existing behavior for valid callbacks.

`tests/automated/check-text-preedit-position-bounds.py` protects this guard.
