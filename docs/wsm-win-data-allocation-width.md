---
nav_exclude: true
---

# WSM window-data allocation width

WSM decodes a bit mask into a variable-length `WSMWinData` array. The decoded
count is now checked against Xt's `Cardinal` allocation width before the
element-size multiplication. Unrepresentable counts follow the existing
allocation-failure cleanup path, preserving stream cleanup and the public
zero-count result.

The regression is covered by
`tests/automated/check-wsm-win-data-allocation-width.py`.
