---
nav_exclude: true
---
# Tooltip popup geometry and lifecycle

Motif creates one tooltip shell and label lazily for each top-level widget.
After the hover delay, the toolkit sets the label text, obtains its preferred
size and positions the complete popup inside the screen. It sizes both the
shell and label **before mapping**. The first visible frame contains the text;
a subtle one-pixel outline surrounds it. The frame, text and padding appear
together at their final size.

`XmNtoolTipPostDelay` still controls the hover delay. The `XmNtoolTipPostDuration`
timer starts when the complete popup is displayed. Leaving the control dismisses
it. Missing children and popup parents remain safe during posting/unposting.
The slide-context slot in the existing configuration trait is retained for ABI
compatibility; tooltip posting does not allocate a slide context.

`tests/automated/check-tooltip-child-failure-guards.py` checks failure guards.
`tests/automated/run-paint.py` also drives real pointer hovers and checks the
first mapped shell/label dimensions, all four outline edges, text pixels in
every sampled frame, stable geometry and dismissal under classic and scaled
modern appearance.
