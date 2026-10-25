---
nav_exclude: true
---
# FontSelector list growth

FontSelector's XLFD and alternate-font lists grow through temporary Xt
allocations. Their capacity checks now cover both host `size_t` and Xt
`Cardinal` byte limits, so an unrepresentable growth request leaves the
existing list unchanged.
