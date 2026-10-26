---
nav_exclude: true
---
# XmString newline entry-count bounds

Newline insertion appends one entry to an internal `int`-counted array.  It
now rejects an existing `INT_MAX` entry count before allocating or incrementing,
avoiding signed overflow while preserving normal newline insertion.

`tests/automated/check-xmstring-newline-count.py` protects the guard.
