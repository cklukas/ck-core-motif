---
nav_exclude: true
---

# DataField selection-array allocation

`df_Validates` now checks the dedicated selection-scan array allocation before
copying the configured entries. If allocation fails, it restores the original
resource/static array pointer and returns without a null `memcpy`.

The focused source contract is checked by
`tests/automated/check-datafield-selection-array.py`.
