---
nav_exclude: true
---
# MultiList layout with an optional find control

When the find controls are requested but the find-text child could not be
created, MultiList now lays out the remaining frame and list without reading
the missing child’s geometry.  A complete find control still receives the
same positioning and width calculation as before.
