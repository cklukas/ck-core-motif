# Mrm decommit-all input

`Idb__BM_DecommitAll` now rejects a null file handle before scanning the shared
buffer pool, avoiding accidental matches against unused entries whose owner is
also null.

The source-level regression is
`tests/automated/check-mrm-decommit-all-input.py`.
