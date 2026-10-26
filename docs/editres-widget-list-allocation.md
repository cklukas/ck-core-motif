---
nav_exclude: true
---
# Edit-resources widget-list allocation

The edit-resources protocol helper now returns safely if its temporary widget
hierarchy allocation fails, rather than writing through a NULL list. Successful
protocol serialization is unchanged.

The focused regression is
`tests/automated/check-editres-widget-list-allocation.py`.
