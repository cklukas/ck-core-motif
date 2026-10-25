---
nav_exclude: true
---
# Mrm resolved path output

Hierarchy loading resolves UID paths through Xt and can produce names longer
than the old 300-byte local scratch buffer.  The resolved-name path now omits
that unused output buffer, and the low-level open helper treats the output name
as optional while preserving existing callers that provide storage.

The source-level regression is
`tests/automated/check-mrm-resolved-path-output.py`.
