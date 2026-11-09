# Tab-list position screen validation

`_XmTabListGetPosition` now returns the existing zero-position fallback when
called without a `Screen`. This prevents unit conversion from dereferencing a
null screen while leaving normal tab positioning unchanged.

The helper now keeps the tab's public floating-point value and relative tab
accumulation in wide local values. Values outside the `int` unit-conversion
contract and totals outside the legacy X11 `Position` range clamp at the
public return boundary instead of narrowing or wrapping. This matters for
large logical tab stops and high-DPI conversions while preserving ordinary
tab coordinates.
