---
nav_exclude: true
---
# Text location cleanup transfer IDs

Text and TextField location-data cleanup callbacks now validate the transfer
callback ID before looking up the destination callback state. A malformed
completion callback therefore exits without querying the transfer subsystem.

Regression coverage: `tests/automated/check-text-location-cleanup-transfer-id.py`.
