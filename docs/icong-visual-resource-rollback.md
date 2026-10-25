---
nav_exclude: true
---
# IconGadget visual-resource rollback

`XmIconGadget` now obtains replacement render tables and label strings before
releasing the current values during `SetValues`.  A failed copy keeps the
existing resource, preventing an allocation failure from leaving the gadget
with a dangling or unusable visual resource.  Geometry measurement also skips
extent calculation when no render table or label is available.

The focused regression is
`tests/automated/check-icong-visual-resource-rollback.py`.
