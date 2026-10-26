---
nav_exclude: true
---

# IconBox cell-query inputs

`XmIconBoxIsCellEmpty` now rejects a null widget before entering the
application context. Invalid optional handles return `False`; valid cell
occupancy queries are unchanged.

The focused regression check is `tests/automated/check-iconbox-inputs.py`.
