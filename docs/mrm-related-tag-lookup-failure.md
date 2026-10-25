---
nav_exclude: true
---
# Mrm primary tag lookup failure handling

A failed compressed primary-tag lookup now skips the rest of that argument.
Previously the conversion could continue into foreground/background checks and
related-count handling using the prior argument’s name slot.  Valid compressed
and unknown tags keep their existing behavior.

The source-level regression is
`tests/automated/check-mrm-related-tag-lookup-failure.py`.
