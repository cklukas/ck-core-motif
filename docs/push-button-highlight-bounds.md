---
nav_exclude: true
---
# PushButton highlight bounds

`XmPushButton` reduces its highlight thickness for default-button shadows and
uses an internal focus delta for the highlight rectangle.  Small thickness or
widget values previously underflowed unsigned arithmetic and could pass an
invalid rectangle or missing GC to `XmeDrawHighlight`.

The focus path now rejects underflow-prone thickness, missing highlight GCs,
and rectangles consumed by the internal delta.  Valid focus highlighting is
unchanged.
