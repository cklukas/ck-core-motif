---
nav_exclude: true
---
# Text drop callback inputs

Text drop handling now fails closed when callback structures, transfer IDs,
drag contexts, or non-empty export-target lists are missing. Valid drops keep
the existing target negotiation and transfer lifecycle.

Regression coverage: `tests/automated/check-text-drop-inputs.py`.
