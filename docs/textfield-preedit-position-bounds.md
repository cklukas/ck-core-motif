---
nav_exclude: true
---

# TextField XIM preedit position bounds

The TextField XIM callback now rejects negative caret, change-start, and
change-length fields before updating its preedit range.  Negative or empty
feedback lengths are also ignored by the rendition helper, and negative text
lengths are rejected before the existing preedit-size limit.  Valid callbacks
retain the existing Motif behavior.

`tests/automated/check-textfield-preedit-position-bounds.py` protects these
guards.
