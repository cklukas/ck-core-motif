---
nav_exclude: true
---

# Parse-mapping ArgList validation

`XmParseMappingGetValues` now rejects a missing `ArgList` when entries were
requested and skips entries without a name or destination pointer. Valid
mapping fields retain their existing behavior, while malformed application
metadata no longer causes a null dereference.

The focused regression check is
`tests/automated/check-parse-mapping-args.py`.
