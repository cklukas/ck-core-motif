---
nav_exclude: true
---

# DataField wide clear-selection allocation

When clearing a wide-character selection, DataField now checks the temporary
space-buffer allocation before filling it. On failure it restores the
insertion-point drawing state and rejects the operation without dereferencing
or freeing a null buffer.

The focused source contract is checked by
`tests/automated/check-datafield-clear-selection.py`.
