---
nav_exclude: true
---

# Mrm Char8 vector span

Mrm byte-string vectors carry both a string-pointer table and string storage.
Before copying and rebasing the pointers, `Urm__CW_SafeCopyValue` now verifies
that the advertised byte span contains every pointer entry.  Truncated or
malformed resource data is rejected before the copy or pointer walk; valid
vectors retain their existing representation and ownership.
