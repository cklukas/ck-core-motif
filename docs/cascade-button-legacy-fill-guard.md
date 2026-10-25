---
nav_exclude: true
---
# CascadeButton legacy fill guard

The legacy etched-in `XmCascadeButton` build branch selected either the armed
or background GC inline and passed it directly to `XFillRectangle`.  Resource
replacement and zero-sized transitions could therefore issue an invalid fill.

That branch now stores the selected GC and validates it and the widget
 dimensions before filling.  The behavior of valid legacy and `FIX_1509`
 builds remains the same.
