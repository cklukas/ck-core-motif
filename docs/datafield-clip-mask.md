---
nav_exclude: true
---

# DataField clip-mask X resources

`df_GetClipMask` now checks both the pixmap and fill-GC creation results before
using them. A failed GC creation releases the pixmap, and a failed pixmap
creation returns immediately, avoiding invalid X requests and null-resource
dereferences.

The focused source contract is checked by
`tests/automated/check-datafield-clip-mask.py`.
