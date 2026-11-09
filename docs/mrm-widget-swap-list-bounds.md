---
nav_exclude: true
---
# Mrm byte-swapped widget list bounds

Before converting a cross-endian widget record's argument, child, or callback
list, Mrm now verifies the record header, descriptor offset, fixed descriptor
prefix, and swapped item count against the serialized record size. Embedded
callback/resource offsets are also checked before a pointer is formed. A
malformed UID record therefore fails with `MrmFAILURE` before conversion can
walk outside its own storage. Correctly formed records retain the existing
in-place byte-swap format and behavior.

The focused source regression is
`tests/automated/check-mrm-widget-swap-list-bounds.py`.
