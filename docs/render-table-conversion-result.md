---
nav_exclude: true
---

# Render-table conversion results

The standard conversion target for a render table serializes the table into a
property buffer.  If serialization reports nonzero bytes without returning a
buffer, the conversion now publishes an empty `None`-typed result instead of
claiming a valid `XA_STRING` payload.  Valid empty and non-empty buffers keep
the existing format and type.

The regression check is
`tests/automated/check-render-table-conversion-result.py`.
