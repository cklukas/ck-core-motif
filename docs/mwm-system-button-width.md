# MWM system-button width validation

`BevelSystemButton()` now validates the available width for each height range
before computing the icon rectangle.  Narrow malformed frame-part rectangles
are skipped instead of wrapping unsigned `width - 4`, `width - 6`, or
`width - 8` calculations.  Normal square title-bar gadgets retain their
existing geometry.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-system-button-width.py
```
