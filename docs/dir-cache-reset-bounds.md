---
nav_exclude: true
---
# Directory-cache reset bounds

Directory-cache reset now checks the qualified-directory length against both
native `size_t` and Xt `Cardinal` limits before narrowing it to the legacy
unsigned cache field and adding the username suffix budget.  Oversized paths
fail cleanly instead of wrapping the allocation size.

`tests/automated/check-dir-cache-reset-bounds.py` protects this contract.
