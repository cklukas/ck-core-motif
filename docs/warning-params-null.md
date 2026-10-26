---
nav_exclude: true
---
# Warning parameter handling

The Motif warning formatter receives Xt-style parameter metadata.  It now
requires both a positive parameter count and a non-NULL parameter array before
copying arguments for `snprintf`.  A malformed warning call with a missing
array therefore falls back to the unparameterized message instead of
dereferencing NULL; normal warning formatting is unchanged.

`tests/automated/check-warning-params-null.py` protects both formatting passes.
