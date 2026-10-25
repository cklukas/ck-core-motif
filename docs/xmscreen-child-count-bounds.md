---
nav_exclude: true
---
# XmScreen child-count bounds

`XmGetXmScreen` now keeps the display child count and scan index in Xt's
`Cardinal` type instead of narrowing them to `int`. Screen lookup behavior is
unchanged while large composite child lists remain representable.
