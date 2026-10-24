# Mrm resource-context output pointer

`UrmGetResourceContext` now rejects a missing context output pointer before
selecting allocators or dereferencing the caller's storage. This also keeps
allocation-failure reporting safe when callers pass invalid output storage.

The source-level regression is `tests/automated/check-mrm-context-output-pointer.py`.
