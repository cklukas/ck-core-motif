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
applications may still provide an explicit size. An unavailable pixmap falls
back to text without loading files, URLs, or a global image cache.

## XmDivider

`XmDivider` is a non-interactive line with an optional copied
`XmNdividerLabel`. `XmNdividerOrientation` selects `XmHORIZONTAL` or
`XmVERTICAL`; `XmNdividerThickness` is clamped to at least one pixel and
`XmNdividerSpacing` controls the gap around a horizontal label. A labelled
horizontal divider draws the line on both sides of the text; a vertical
divider remains a line and does not rotate or reinterpret the label.

Both controls expose a label role and copied text through the generic
accessibility bridge. Applications own placement, visibility and lifetime.
Destroying either widget does not destroy a supplied pixmap.

The installed consumer fixture is `tests/automated/run-icon-label-divider.py`.
