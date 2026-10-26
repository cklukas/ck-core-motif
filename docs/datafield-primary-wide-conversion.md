---
nav_exclude: true
---

# DataField primary-selection wide conversion allocation

The wide primary-selection conversion in `df_DoStuff` now checks its temporary
wide buffer before conversion. On allocation failure it releases the converted
text list, callback value, and request state, and restores the temporary
selection-move max-length override.

The focused source contract is checked by
`tests/automated/check-datafield-primary-wide-conversion.py`.
