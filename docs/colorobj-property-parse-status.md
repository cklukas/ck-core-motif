---
nav_exclude: true
---

# ColorObj property parse status

ColorObj property lookup now reports whether the serialized pixel data was
actually parsed.  Malformed data or allocation failure returns failure to the
cache lookup, allowing the existing selection-based fallback to run.

Valid properties retain the same update behavior.  The static regression check
is `tests/automated/check-colorobj-property-parse-status.py`.
