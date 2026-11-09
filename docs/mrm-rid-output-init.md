---
nav_exclude: true
---
# Mrm RID allocator output initialization

The direct header and RID-record resource-ID allocators now clear a supplied
output ID before validating file state or attempting to extend the RID map. A
failed allocation therefore cannot leave callers with an older ID that could be
mistaken for a newly reserved resource. Successful resource-ID allocation and
on-disk layout are unchanged.

The source-level regression is
`tests/automated/check-mrm-rid-output-init.py`.
