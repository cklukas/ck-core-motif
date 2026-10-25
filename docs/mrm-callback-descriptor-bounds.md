---
nav_exclude: true
---
# Mrm callback descriptor bounds

Mrm argument and creation callback constructors now keep descriptor sizes in
`size_t` and reject counts whose complete records cannot fit in the format's
16-bit `MrmSize` field. Valid callback counts retain their existing record
layout and terminating runtime entry.

The source-level regression is `tests/automated/check-mrm-callback-descriptor-bounds.py`.
