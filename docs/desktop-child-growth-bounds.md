# Desktop child-list growth bounds

`XmDesktop` now checks `Cardinal` slot-count growth and the `size_t` byte
conversion before reallocating its child list. It also preserves the old list
when `XtRealloc` fails. Existing insertion and deletion ordering is unchanged,
but extreme counts and allocation failures no longer risk wraparound or a
lost child list.
