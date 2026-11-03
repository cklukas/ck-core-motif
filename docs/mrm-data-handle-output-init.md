---
nav_exclude: true
---
# MRM data-handle failure outputs

The internal MRM helpers that produce an `IDBDataHandle` now clear its record
number and item offset before validating inputs, accessing IDB records, or
allocating storage. A failed lookup or write therefore cannot cause a caller to
reuse an earlier successful handle as though it described the failed operation.

The source-level regression is
`tests/automated/check-mrm-data-handle-output-init.py`.
