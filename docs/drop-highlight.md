# DropHighlight

`XmDropHighlight` is an opt-in presentation overlay for an existing drop
target.  The application keeps its current `XmDropSiteRegister` contract and
calls `XmDropHighlightSetState` from its existing enter, leave and acceptance
paths.  The widget never registers a drop site, accepts a transfer, changes
the drop operation or invokes a drop callback.

The `XmDROP_HIGHLIGHT_VALID` state draws a solid outline and
`XmDROP_HIGHLIGHT_INVALID` draws a dashed outline.  `XmDROP_HIGHLIGHT_NONE`
clears the outline.  This deliberately uses line style as well as color so a
high-contrast theme can distinguish accepted and rejected targets without
depending on a particular palette.  The presentation is static and has no
timer, animation or pointer-grab side effects.

The application owns geometry and placement.  An optional copied
`XmNdropHighlightLabel` is exposed as the accessible panel name; no
accessibility action is advertised because acceptance and transfer remain
application-owned.
