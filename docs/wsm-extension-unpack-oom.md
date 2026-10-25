---
nav_exclude: true
---
# WSM extension unpack allocation failure

WSM request and reply extension lists now check their pointer-array
allocations before filling or marking the structures owned. The request's
connection-version array follows the same fail-closed contract. On failure
the count and pointer are cleared and unpacking returns, so dispatch and
cleanup cannot walk a null array.

`tests/automated/check-wsm-extension-unpack-oom.py` protects both unpackers.
