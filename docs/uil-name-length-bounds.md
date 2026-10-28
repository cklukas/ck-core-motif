---
nav_exclude: true
---

# UIL name-length bounds

Name entries use a legacy unsigned-short allocation-size API and a compact
character subtype field. Name lookup and insertion now reject null, negative,
or over-wide lengths before hashing or allocation, and the validated lengths
are converted explicitly for copying and record storage.

Regression: `tests/automated/check-uil-name-length-bounds.py`
