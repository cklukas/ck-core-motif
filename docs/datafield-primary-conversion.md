---
nav_exclude: true
---

# DataField primary-selection conversion allocation

The narrow primary-selection conversion in `df_DoStuff` now checks its
aggregate buffer before writing the concatenated text. On allocation failure,
the converted text list, callback value, and request state are released, and a
temporary selection-move max-length override is restored.

The focused source contract is checked by
`tests/automated/check-datafield-primary-conversion.py`.
