---
nav_exclude: true
---

# XPM array end-of-input guard

Array-backed XPM data is a null-terminated list of strings.  The scanner now
checks both the array pointer and the next line pointer before dereferencing
them.  Malformed or truncated arrays therefore terminate parsing as invalid
input instead of turning a missing line into a null-pointer read.

Regression: `tests/automated/check-xpm-array-end-guard.py`
