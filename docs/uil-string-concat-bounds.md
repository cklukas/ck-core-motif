---
nav_exclude: true
---

# UIL string-concatenation bounds

UIL simple-string values store their length in a 16-bit symbol-entry field.
Concatenation now rejects a pair whose combined length cannot fit that field
before allocating or copying, preventing signed arithmetic and narrowing from
turning a large valid pair into a wrapped allocation or length.

Regression: `tests/automated/check-uil-string-concat-bounds.py`
