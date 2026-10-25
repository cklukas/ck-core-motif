# RCMenu post-from-list growth

Shared popup menus keep their attach widgets in a dynamically allocated
`postFromList`. The list uses signed `int` counters for ABI compatibility.

The growth path now treats a count at or beyond capacity as a growth request,
checks signed and byte-size limits, and publishes the replacement pointer and
capacity only after `XtRealloc` succeeds. Existing attach widgets remain valid
when allocation is unavailable or the counters are malformed.
