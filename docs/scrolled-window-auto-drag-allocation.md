---
nav_exclude: true
---
# ScrolledWindow auto-drag allocation

The horizontal and vertical auto-drag rectangle helpers now initialize their
outputs to an empty set and check `XtMalloc` before filling the rectangles.
Allocation failure therefore disables that auto-drag geometry for the call
without dereferencing a null pointer or exposing a nonzero count with no
storage. Successful behavior remains unchanged.
