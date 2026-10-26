---
nav_exclude: true
---

# DataField paste conversion safety

`XmDataFieldPaste` now bounds the accumulated clipboard strings before narrowing
them to Xt's `Cardinal` allocation size. Both narrow and wide conversion
buffers handle allocation failure without dereferencing a null pointer. Wide
conversion also checks `mbstowcs` using its `size_t` error sentinel and rejects
character counts that cannot be passed through the existing `int` replacement
API.

The focused source contract is checked by
`tests/automated/check-datafield-paste-conversion.py`.
