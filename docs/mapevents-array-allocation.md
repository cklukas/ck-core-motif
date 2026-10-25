---
nav_exclude: true
---
# Key-event mapping array allocation

`_XmMapKeyEvents` returns three parallel arrays while parsing a virtual-key
translation. Each array resize is now checked before its new element is
written, and count-to-byte arithmetic is bounded. A failure marks the parse
as unsuccessful so the existing cleanup path releases every partial array and
returns an empty result.

Successful mappings keep the same array layout and ordering. The change only
prevents allocation pressure or integer limits from becoming a null-pointer
write or mismatched parallel arrays.
