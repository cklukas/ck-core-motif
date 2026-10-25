---
nav_exclude: true
---
# DrawnButton shadow redisplay guards

`XmDrawnButton` has two shadow-rendering paths: the ordinary drawn-button path
and the optional push-button emulation path.  Both previously passed cached
GCs and dimensions directly to `XmeDrawShadows`, including during realization
or small-geometry transitions.

Both paths now share a guard that requires a realized widget, both manager
shadow GCs, non-zero shadow thickness, and a drawable rectangle after
highlight margins.  Label exposure and application expose callbacks continue
to run even when an intermediate shadow draw is skipped.
