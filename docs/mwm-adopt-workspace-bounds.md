# MWM initial-client workspace bounds

When WSM adopts already-managed windows, it builds an ancillary-window array
containing two fixed windows plus one entry per workspace.  The count is now
validated before the signed addition and `XtMalloc` size calculation.  Invalid
or unrepresentable workspace counts terminate adoption cleanly instead of
allowing integer or allocation-size overflow. If the ancillary array cannot be
allocated, adoption now returns after requesting shutdown rather than assuming
the shutdown hook cannot return. XQueryTree traversal indexes also use its
returned unsigned count type.
