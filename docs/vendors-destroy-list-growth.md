---
nav_exclude: true
---
# VendorS destroy-grab list growth

Vendor shell extensions keep a small registry of shells that need destroy
callbacks.  The old growth path incremented its 16-bit capacity and assigned
`XtRealloc`'s result without checking it, so a failed allocation could lose
the registry and write through a null pointer.

Growth now checks both the counter and byte arithmetic, then commits the new
pointer and capacity only after successful allocation.
