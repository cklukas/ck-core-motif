---
nav_exclude: true
---

# WSM list-payload allocation width

WSM window-data records can carry lists of 8-, 16-, or 32-bit values. The
decoder now checks each wire count against Xt's `Cardinal` allocation width
before multiplying by the native element size. An unrepresentable or failed
allocation follows the existing payload-consumption path, leaving a clean
empty value instead of risking a wrapped allocation.

The regression is covered by
`tests/automated/check-wsm-list-payload-allocation-width.py`.
