---
nav_exclude: true
---
# Compound-string clip rectangles

`XmStringDraw` honors a supplied clip rectangle even when a text line lies
entirely outside it. This applies to core X fonts and Xft render tables.
A disjoint rectangle suppresses the line; it does not turn clipping off.

The previous intersection optimization could skip installing a disjoint clip
and draw the entire line. Rendering the same string in two colors over separate
filled/unfilled regions could therefore let the second pass overwrite the first.
The renderer now installs the requested region consistently, including fully
containing and partially intersecting rectangles. There is no API or widget-record
layout change, and the correction applies to every appearance profile.

`check-xmstring-disjoint-clip.c` checks both font backends against horizontal and
vertical disjoint rectangles, a partial rectangle and an enclosing rectangle.
The modern control regression also measures the actual progress-label pixels
against the fill color and requires at least 4.5:1 contrast at 1× and 2× in both
modern profiles. These tests are included in `run-modern-macos-audit.py`.
