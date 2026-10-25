---
nav_exclude: true
---
# FontS list-size bounds

The font selector grows its XLFD and miscellaneous font arrays in fixed
increments.  Their signed counters were checked for increment overflow, but
the resulting slot count was not checked before multiplication by the element
size.

Both growth helpers now reject byte counts that cannot be represented by
`size_t`, preserving the existing arrays and list counters.
