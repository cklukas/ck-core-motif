---
nav_exclude: true
---
# FileSelectionBox initialization-copy rollback

`XmFileSelectionBox` now checks its initial copies of the no-match, directory,
and pattern strings. Failed copies retain the original resource instead of
publishing NULL or measuring it. A failed directory-mask copy falls back to
the standard `*` mask when possible, and string lengths are only measured for
non-NULL copies.

The focused regression is
`tests/automated/check-filesb-init-copy-rollback.py`.
