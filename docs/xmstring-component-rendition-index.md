---
nav_exclude: true
---
# XmString optimized rendition indices

`XmeStringGetComponent` now validates optimized rendition indices and cache
entries before indexing the shared tag cache. Malformed or stale values mark
the context failed and return `XmSTRING_COMPONENT_END` instead of asserting or
performing out-of-bounds pointer arithmetic.

The regression check is
`tests/automated/check-xmstring-component-rendition-index.py`.
