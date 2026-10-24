# Mrm file-open input validation

The low-level Mrm file opener now rejects null filename, output-name, and file
handle pointers before dereferencing them. It also keeps the filename length in
`size_t` through the copy and allocation paths, avoiding truncation on long
filesystem names while preserving normal read and write behavior.

The source-level regression is `tests/automated/check-mrm-file-open-input.py`.
