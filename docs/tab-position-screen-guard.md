# Tab-list position screen validation

`_XmTabListGetPosition` now returns the existing zero-position fallback when
called without a `Screen`. This prevents unit conversion from dereferencing a
null screen while leaving normal tab positioning unchanged.
