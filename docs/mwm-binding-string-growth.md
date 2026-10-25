# MWM binding-string growth

MWM expands virtual-key bindings by replacing matching names with longer
names. The expansion now computes prefix, suffix, and replacement sizes without
modifying the live string first, checks the Xt size limit, and commits the
replacement pointer only after `XtRealloc` succeeds.

If growth cannot be represented or allocated, the current binding string is
returned unchanged and the caller can continue using the prior bindings.
