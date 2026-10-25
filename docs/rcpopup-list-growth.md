---
nav_exclude: true
---
# RCPopup list growth failures

The row-column popup registry grows a per-widget `WidgetList` whenever a
menu is posted.  The old path assigned `XtRealloc`'s result directly and
then stored the new parent widget, so an allocation failure discarded the
registry and caused a null-pointer write.

Growth now checks both host `size_t` and Xt's `Cardinal` byte limits before
multiplication, passes an explicit Xt-sized allocation, and commits the new
list only after allocation succeeds. Existing popup registrations stay valid
when the list cannot grow.
