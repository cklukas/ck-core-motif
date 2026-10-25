---
nav_exclude: true
---
# Mrm argument-list bounds

Mrm value updates and widget creation now check Arg-list counts before adding
UID arguments or allocating the Xt argument arrays. Both host `size_t` and
Xt's `Cardinal` byte limits are enforced. Unrepresentable counts return
`MrmFAILURE` without wrapping; ordinary UID argument conversion and widget
creation are unchanged.

The source-level regression is `tests/automated/check-mrm-arg-bounds.py`.
