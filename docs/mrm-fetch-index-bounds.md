---
nav_exclude: true
---
# Mrm fetch-set-values index bounds

`UrmFetchSetValues` builds a reusable resource descriptor from the longest
literal index supplied by the application. The helper now rejects names that
would exceed the 16-bit Mrm resource-size limit or wrap the allocation
addition before allocating the descriptor. Valid literal fetches retain their
existing lookup and conversion behavior.

The source-level regression is
`tests/automated/check-mrm-fetch-index-bounds.py`.
