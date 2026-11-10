---
nav_exclude: true
---
# Edit-resources widget-list allocation

The hierarchy of a widget written to a reply is checked against the 16-bit
wire field, uses a small stack array for ordinary depths and marks the reply
failed instead of writing a partial path when memory is short. Successful
protocol serialization is unchanged.

The focused regression is
`tests/automated/check-editres-widget-list-allocation.py`.
