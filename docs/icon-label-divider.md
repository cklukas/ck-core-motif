# IconLabel and Divider

`XmIconLabel` and `XmDivider` are additive presentation controls for compact
content layouts. They do not change `XmLabel`, `XmSeparator`, button geometry,
or pixmap ownership.

## XmIconLabel

`XmIconLabel` owns a copied `XmString` in `XmNiconLabelLabel`. An optional
`XmNiconLabelPixmap` is borrowed from the application and is drawn only while
the server-side pixmap remains valid; `XmUNSPECIFIED_PIXMAP` is the explicit
no-icon value. `XmNiconLabelSpacing` separates icon and text. The inherited
`XmNlayoutDirection` controls whether the icon is placed before or after the
label. It is non-interactive and has no implicit host placement or callback.

Its geometry is negotiated from the label text and valid pixmap dimensions;
multiline compound text uses its measured height and padding follows DPI.
An explicit width or height remains application-owned, including an assignment
of its current value; setting a dimension to zero restores automatic sizing.
Live font/scale changes recompute automatic dimensions. The icon and text order
mirrors in RTL, and no inter-item gap is reserved for an empty label. Modern
default icon spacing follows DPI and density (6 logical pixels comfortable,
4 compact), while explicit spacing remains application-owned.
Drawing uses current theme and sensitivity colors. Applications may still
provide an explicit size. An unavailable pixmap falls
back to text without loading files, URLs, or a global image cache.

Icon and text drawing remain inside the padded allocation. Oversized icons are
cropped by intersecting source and destination rectangles before copying; text
uses only the remaining area beside the icon. Narrow or short allocations cannot
paint into padding or into the icon's reserved area. Text aligns to the beginning
of that area in both left-to-right and right-to-left layouts.

`tests/automated/run-iconlabel-layout.py --build /absolute/build --out
/absolute/fresh-output` checks actual rendered pixel bounds for text-only,
icon-only and combined content. It covers both modern themes, three scales, both
layout directions, constrained allocations, multiline text, automatic-size reset
and explicit geometry ownership.

## XmDivider

`XmDivider` is a non-interactive line with an optional copied
`XmNdividerLabel`. `XmNdividerOrientation` selects `XmHORIZONTAL` or
`XmVERTICAL`; `XmNdividerThickness` is clamped to at least one pixel and
`XmNdividerSpacing` controls the gap around a horizontal label. A labelled
horizontal divider draws the line on both sides of the text; a vertical
divider remains a line and does not rotate or reinterpret the label.

Natural dimensions include the measured multiline label, line thickness and
scaled padding. Modern default label spacing is 8 logical pixels in comfortable
density and 6 in compact density; explicit spacing remains application-owned.
The labeled and unlabeled line paths both honor thickness. Modern lines use
the theme's bottom-shadow colour while the label uses the foreground colour.
Drawing refreshes live colours and clips text to the allocation. When space is
too narrow for a line segment beside the label, that segment is omitted.

Explicit width and height are independently preserved. Set either to zero to
restore automatic sizing; content, font, spacing, orientation and scale changes
then recompute it. Both measured dimensions saturate rather than wrapping when
large resources exceed the Dimension range. The gallery uses these defaults
without custom dimensions. `run-divider-geometry.py` covers native geometry,
line pixels, RTL, palette changes, tiny allocations and overflow bounds.

Both controls expose a label role and copied text through the generic
accessibility bridge. Applications own placement, visibility and lifetime.
Destroying either widget does not destroy a supplied pixmap.

The installed consumer fixture is `tests/automated/run-icon-label-divider.py`.
