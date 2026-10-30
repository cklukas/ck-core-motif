---
nav_exclude: true
---
# ColorSelector mode values

`XmNcolorMode` accepts the public `XmListMode` and `XmScaleMode` values.  A
caller can still bypass the string converter and supply an arbitrary integer
through an `Arg` list.  The mode transition path now normalizes such a value
to `XmScaleMode` before indexing the two-element toggle array or changing
managed children.

Valid resource values and the existing string converter are unchanged; this
only makes direct malformed values recoverable instead of allowing an
out-of-bounds array access.
