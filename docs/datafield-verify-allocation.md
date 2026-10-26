---
nav_exclude: true
---

# DataField modify-verify allocation

When a DataField has a narrow-character modify-verify callback,
`df_ModifyVerify` creates a temporary text block for the callback. The helper
now returns `False` immediately if that allocation fails, before copying input
text into the missing buffer. This preserves the edit-rejection contract and
avoids a null dereference under memory pressure.

The focused source contract is checked by
`tests/automated/check-datafield-verify-allocation.py`.
