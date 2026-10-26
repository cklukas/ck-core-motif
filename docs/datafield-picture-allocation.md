---
nav_exclude: true
---

# DataField picture verification allocation

`PictureVerifyCallback` now rejects a failed splice-buffer allocation before
copying the current value or proposed edit into it. The callback marks the
edit as rejected and releases the current string, preserving the callback's
existing failure semantics without dereferencing a null buffer.

The focused source contract is checked by
`tests/automated/check-datafield-picture-allocation.py`.
