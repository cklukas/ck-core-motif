---
nav_exclude: true
---
# Text selection target-list inputs

Text and TextField target-list callbacks now reject a missing target-list
buffer even when the reported target count is non-zero. This prevents a
malformed or teardown-time selection callback from dereferencing a null list.

Regression coverage: `tests/automated/check-text-selection-target-list-inputs.py`.
