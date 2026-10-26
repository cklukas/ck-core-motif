---
nav_exclude: true
---
# Text selection target counts

The external target-list callback now rejects counts larger than the legacy
selection-processing limit before iterating the supplied atom array. Normal
target negotiation is unchanged, while oversized external metadata cannot wrap
the iteration index.

Regression coverage: `tests/automated/check-text-selection-target-count.py`.
