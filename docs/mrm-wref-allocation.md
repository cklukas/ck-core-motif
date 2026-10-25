---
nav_exclude: true
---
# Mrm widget-reference allocation handling

Set-values widget-reference and callback-reference builders now check pointer
list initialization, descriptor/name allocation, callback-copy allocation, and
pointer-list append results.  Failure paths release allocations they own while
leaving compressed tag-name storage untouched.

The source-level regression is `tests/automated/check-mrm-wref-allocation.py`.
