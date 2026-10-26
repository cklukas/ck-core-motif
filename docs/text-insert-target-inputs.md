---
nav_exclude: true
---
# Text insert-target inputs

Text and TextField insert-target callbacks now validate widget, closure,
target-list, and transfer context inputs before iterating targets or requesting
the selected target. This prevents malformed secondary-selection responses
from reaching the transfer API.

Regression coverage: `tests/automated/check-text-insert-target-inputs.py`.
