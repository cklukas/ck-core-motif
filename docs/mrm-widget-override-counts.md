---
nav_exclude: true
---

# Mrm widget override counts

Widget creation and automatic-child set-values accept a public Xt `Cardinal`
override count.  Their temporary allocation accounting now retains that width
as `size_t` and uses a `Cardinal` copy index, rather than first narrowing the
count to Mrm's 16-bit on-disk `MrmCount` type.  Checked aggregate capacity
arithmetic remains in place before the argument-array allocation, so large
valid override lists either allocate safely or fail cleanly without wrapping
the list size.

The focused regression is
`tests/automated/check-mrm-widget-override-counts.py`.
