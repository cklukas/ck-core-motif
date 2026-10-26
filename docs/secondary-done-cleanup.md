---
nav_exclude: true
---

# Secondary conversion cleanup

`SecondaryDone` can run after the secondary conversion context has been
cleared by selection teardown.  The callback may still contain an allocated
selection value even though no conversion callback can be dispatched.  The
cleared-context path now releases that value before disowning the selection,
matching the normal completion path and preventing a teardown-time leak.

The regression check is `tests/automated/check-secondary-done-cleanup.py`.
