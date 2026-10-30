---
nav_exclude: true
---
# Notebook redisplay constraint guards

Notebook redisplay now skips null composite children and children without
constraint storage while counting major and minor tabs. Expose handling no
longer dereferences malformed child state; normal tab-count and scroller
behavior is unchanged.

The source contract is checked by
`tests/automated/check-notebook-redisplay-constraint-guards.py`.
