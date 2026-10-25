---
nav_exclude: true
---
# Obso2 translation-buffer growth

The compatibility `_XmGetRealXlations` helper appends virtual-key text to a
growable buffer.  Its growth helper previously overwrote the buffer pointer
with an unchecked `XtRealloc` result, and a zero initial capacity could never
grow.

The helper now starts zero-capacity buffers at one byte, commits the new
pointer only after successful allocation, and reports failure through the
existing caller cleanup path.
