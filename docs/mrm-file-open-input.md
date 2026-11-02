---
nav_exclude: true
---
# Mrm file-open input validation

The low-level Mrm file opener now rejects null filename, output-name, and file
handle pointers before dereferencing them. It also keeps the filename length in
`size_t` through the measured copy path, then converts the already-validated
allocation size explicitly to Xt's `Cardinal` contract. This avoids implicit
truncation on long filesystem names while preserving normal read and write
behavior.

The source-level regression is `tests/automated/check-mrm-file-open-input.py`.
