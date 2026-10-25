---
nav_exclude: true
---
# MWM label-parse allocation

MWM resource parsing copies menu labels and quoted client-command names before
checking for bitmap syntax. It now rejects null inputs and checks both `size_t`
and Xt `Cardinal` limits before allocating either string, avoiding a narrow wrap
while preserving normal label and bitmap handling.

`tests/automated/check-mwm-label-parse-allocation.py` protects this contract.
