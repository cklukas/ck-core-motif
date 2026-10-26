---
nav_exclude: true
---
# DataField empty substring

`XmDataFieldGetSubstring` now treats a zero-length, in-range request as a
successful copy and writes an empty string to the caller's buffer. This matches
the wide-character API and keeps empty field selections usable.

The focused source check is
`tests/automated/check-datafield-substring-range.py`.
