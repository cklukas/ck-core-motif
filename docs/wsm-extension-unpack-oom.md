# WSM extension unpack allocation failure

WSM request and reply extension lists now check their pointer-array
allocations before filling or marking the structures owned. On failure they
clear the count and pointer and return, so dispatch and cleanup cannot walk a
null array.

`tests/automated/check-wsm-extension-unpack-oom.py` protects both unpackers.
