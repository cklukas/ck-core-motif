---
nav_exclude: true
---
# Mrm resource-context output pointer

`UrmGetResourceContext` rejects a missing context output pointer before
selecting allocators or dereferencing the caller's storage. Once validated, it
clears the output before allocation and clears it again after releasing a
partially allocated context when its data buffer cannot be allocated. Failed
construction therefore cannot leave a stale or freed context handle with the
caller.

The source-level regression is `tests/automated/check-mrm-context-output-pointer.py`.
