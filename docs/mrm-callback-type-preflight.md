---
nav_exclude: true
---

# Mrm callback item type preflight

`UrmCWRSetCallbackItem` now validates its representation type before appending
the routine name or updating the callback item. Unsupported vector and unknown
types therefore fail without leaving record data behind, while the existing
scalar, string, wide-character, floating-point, and null callback forms remain
unchanged.
