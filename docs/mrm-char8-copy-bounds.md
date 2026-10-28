---
nav_exclude: true
---
# Mrm Char8 copy bounds

Mrm's safe resource-copy path now keeps a `Char8` string length in `size_t`
until it has been checked against Xt's `Cardinal` allocation width. An
unrepresentable string is rejected cleanly; valid strings retain the same
null-terminated copy behavior.

The source contract is covered by
`tests/automated/check-mrm-char8-copy-bounds.py`.
