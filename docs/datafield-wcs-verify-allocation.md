---
nav_exclude: true
---

# DataField wide modify-verify allocation

`df_ModifyVerify` now checks the temporary wide-character callback buffer in
both conversion branches before `mbstowcs`, `memcpy`, or terminator writes.
When the narrow callback ran first, its temporary buffer is released before
rejecting the edit; the direct wide-input branch has no temporary ownership to
release. This keeps callback failure safe under allocation pressure.

The focused source contract is checked by
`tests/automated/check-datafield-wcs-verify-allocation.py`.
