---
nav_exclude: true
---
# XIM callback slot validation

Input-method callback registration stores callbacks in a fixed four-slot
array.  The internal helper now rejects a null callback or an out-of-range
slot before indexing it.  Valid preedit callback registration is unchanged.

`tests/automated/check-xim-callback-index.py` protects this contract.
