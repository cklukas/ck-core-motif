---
nav_exclude: true
---
# MWM label-parse allocation

MWM resource parsing copies menu labels before checking for bitmap syntax.  It
now rejects null inputs and uses the full `size_t` length for the allocation,
avoiding a narrow `unsigned int` wrap while preserving normal label and bitmap
handling.

`tests/automated/check-mwm-label-parse-allocation.py` protects this contract.
