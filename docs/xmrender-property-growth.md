---
nav_exclude: true
---
# XmRenderTable property-buffer growth

Render-table serialization appends formatted properties to a dynamically
grown character buffer.  The previous macro assigned `XtRealloc`'s result
directly and did not enforce Xt's `Cardinal` size limit.

The macro now checks the limit, commits the replacement only after successful
allocation, and uses its existing cleanup return when growth fails.
