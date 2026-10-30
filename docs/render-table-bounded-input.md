---
nav_exclude: true
---

# Bounded render-table import

`XmRenderTableCvtFromProp` now honors its `len` argument by copying exactly
that many bytes into a NUL-terminated parser buffer. This prevents the token
scanner from reading beyond a property that is not NUL-terminated while
preserving the public conversion API and its existing token grammar.

The focused regression check is
`tests/automated/check-render-table-bounded-input.py`.
