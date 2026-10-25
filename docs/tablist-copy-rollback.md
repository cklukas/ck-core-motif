---
nav_exclude: true
---
# Tab-list copy rollback

When a later tab cannot be copied, `XmTabListCopy` now closes the partial ring,
records the number of successfully copied tabs, unlocks process state, and
frees the partial list before returning `NULL`. Existing marked-tab ownership
rules remain unchanged.
