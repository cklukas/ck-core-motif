---
nav_exclude: true
---
# Mrm literal output validation

`MrmFetchLiteral` returns both a literal value and its `MrmCode` through caller
owned output pointers. The legacy implementation entered the application and
process locks, fetched the resource, and dereferenced those pointers without
checking them. Passing either output as `NULL` could therefore crash the
process instead of returning the documented failure status.

The routine now rejects missing output pointers before acquiring locks or
allocating a resource context, and clears both outputs before attempting the
fetch. Existing callers that provide valid outputs retain the same lookup and
conversion behavior. The source-level regression is
`tests/automated/check-mrm-fetch-literal-output.py`.
