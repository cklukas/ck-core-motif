# XmList read-only query null-safety

The read-only list queries `XmListItemExists`, `XmListItemPos`,
`XmListGetKbdItemPos`, and `XmListYToPos` now reject null widgets before
entering Xt app-context handling. Item-based queries also reject null items.
They return their documented empty result (`False` or `0`) while valid calls
retain their existing behavior.
