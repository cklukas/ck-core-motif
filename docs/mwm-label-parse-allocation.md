---
nav_exclude: true
---
# MWM label-parse allocation

MWM resource parsing copies menu labels before checking for bitmap syntax.  It
now rejects null inputs and checks both `size_t` and Xt `Cardinal` limits before
allocating the label, avoiding a narrow wrap while preserving normal label and
bitmap handling.

`tests/automated/check-mwm-label-parse-allocation.py` protects this contract.
