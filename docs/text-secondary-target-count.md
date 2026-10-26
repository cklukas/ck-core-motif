---
nav_exclude: true
---
# Text secondary-selection target counts

Secondary-selection target negotiation now rejects external target counts that
cannot be safely processed by the legacy transfer path, frees the received
target list, and fails the transfer before scanning it.

Regression coverage: `tests/automated/check-text-secondary-target-count.py`.
