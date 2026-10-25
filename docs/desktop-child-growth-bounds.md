---
nav_exclude: true
---
# Desktop child-list growth bounds

`XmDesktop` now checks `Cardinal` slot-count growth and both host `size_t` and
Xt `Cardinal` byte limits before reallocating its child list. It also preserves the old list
when `XtRealloc` fails. Existing insertion and deletion ordering is unchanged,
but extreme counts and allocation failures no longer risk wraparound or a
lost child list.
