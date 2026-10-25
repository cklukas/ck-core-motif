# Mrm indexed-resource enumeration output validation

`UrmIdbFindIndexedResource` appends matching index strings to a caller-owned
pointer list. It now rejects a null list before validating the file or taking
the empty-database fast path, preventing a malformed call from reaching the
index walker with no output destination.

The source-level regression check is
`tests/automated/check-mrm-find-indexed-output.py`.
