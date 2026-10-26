---
nav_exclude: true
---
# TextField export-target conversion allocation

Selection conversion now checks the `Atom` array used for Motif export-target
responses.  If it cannot be allocated, the transfer completes with an empty
failed response instead of writing through a null pointer.

Regression coverage: `tests/automated/check-textfsel-export-targets-allocation.py`.
