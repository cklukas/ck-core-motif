---
nav_exclude: true
---
# Transfer done-proc inputs

Transfer completion dispatch now validates the transfer context, completion
record, callback count, and callback list before iterating done procedures.
Malformed internal completion paths therefore return safely.

Regression coverage: `tests/automated/check-transfer-done-proc-inputs.py`.
