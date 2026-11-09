---
nav_exclude: true
---
# Outline child-operation list allocation

`XmOutline` uses its private child-operation list from constraint destruction,
layout, redraw, and work-procedure callbacks. Initialization and replacement of
that list now stop with an Xt error if allocation fails, rather than leaving a
partially initialized widget whose later callbacks dereference a null list.
Successful layout and list ownership are unchanged.

The focused regression check is
`tests/automated/check-outline-initialize-list.py`.
