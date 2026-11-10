# MWM text drawing length bounds

`XTextWidth`, `XDrawString`, and `XDrawImageString` take signed `int` text
lengths.  MWM's `DrawStringInBox` now measures the input string once, rejects
lengths above `INT_MAX`, and uses the checked `int` value consistently for
measurement and drawing.  `WmDrawString` has the matching signed interface and
also rejects a negative length.

This preserves normal 8-bit MWM title and feedback drawing while preventing a
large string from narrowing into an invalid Xlib count.

Focused check:

```
python3 tests/automated/check-mwm-draw-string-length.py
```
