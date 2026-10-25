---
nav_exclude: true
---
# Menu button border-clear bounds

Menu leave and disarm callbacks can run while a button has less space than its
highlight frame.  CascadeButton, PushButton, and ToggleButton now skip
`XmeClearBorder` unless both interior dimensions are positive, and convert the
unsigned core dimensions before subtracting the frame.

Normal-sized menu entries retain their existing border clearing and callback
ordering.  The focused regression is
`tests/automated/check-menu-border-clear-bounds.py`.
