---
nav_exclude: true
---
# XmStringGenerate fallback rendition decoration

When rendition caching cannot use the optimized representation,
`XmStringGenerate` now checks both rendition-component allocations and both
concatenations. Any failure frees the partial result and returns `NULL` rather
than returning an undecorated string.

The regression check is
`tests/automated/check-xmstring-generate-rendition-fallback.py`.
