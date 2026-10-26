---
nav_exclude: true
---

# Complete inactive-tab outlines

`XmTabBox` now draws the outer outline for every inactive tab, including tabs
on later rows of a stacked layout.  The old renderer omitted those edges when
the tab was not on row zero, leaving labels such as **Source** or **Built-in
Icons** visually detached from the tab control.  The existing Motif shadow
GCs remain responsible for the subdued outline, and selected tabs retain
their existing selected-state fill and bevel.
