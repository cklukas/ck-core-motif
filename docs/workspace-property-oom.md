---
nav_exclude: true
---
# WSM workspace-property allocation failures

WSM workspace hints and workspace-list properties now check their atom-array
sizes before converting them to Xt’s `Cardinal` allocator and handle NULL
returns without copying or publishing invalid data. Hint callers receive
`BadAlloc`; list publication is skipped with a warning when it cannot be
represented or allocated.

`tests/automated/check-workspace-property-oom.py` guards these failure paths.
