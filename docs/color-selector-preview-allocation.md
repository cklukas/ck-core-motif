---
nav_exclude: true
---
# ColorSelector preview allocation

ColorSelector treats its color preview as an optional internal child.  If a
resource hook or an allocation failure prevents that child from being created,
color-name parsing and colormap bookkeeping still complete without calling Xt
with a null widget.  Preferred-size measurement skips the missing preview
parent, and layout and color-display updates simply omit the visual update.

This preserves the selector's recoverable behavior during partial construction:
the list, sliders, and radio controls remain usable, and a later relayout does
not turn a failed preview allocation into a crash.
