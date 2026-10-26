---
nav_exclude: true
---
# Text secondary-selection transfer failure

Malformed secondary-selection callbacks now fail an active transfer when a
transfer ID is available, instead of returning silently and leaving the
transfer pending. Null callback state remains safe when no ID exists.

Regression coverage: `tests/automated/check-text-secondary-transfer-failure.py`.
