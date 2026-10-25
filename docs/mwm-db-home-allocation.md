---
nav_exclude: true
---
# MWM database filename allocation

When no session database path is configured, MWM derives one from the home
directory.  The lookup is now checked before measuring or formatting it, and
the combined path length is bounded against both `size_t` and Xt's `Cardinal`
width before allocation.  If the home directory
is unavailable or unrepresentable, MWM leaves the database path unset for the
existing caller fallback instead of dereferencing null or wrapping a size.

`tests/automated/check-mwm-db-home-allocation.py` protects this contract.
