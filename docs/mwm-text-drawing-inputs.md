# MWM text drawing input guards

The legacy text helpers now reject incomplete input records before calling
Xlib:

- `DrawStringInBox()` checks its display, graphics context, font, box, and
  string pointers.
- `WmDrawString()` checks its display, graphics context, string, and active
  screen pointer.

Valid 8-bit title and feedback text keeps the existing centering, clipping,
and clean-text behavior.  Invalid teardown or partially initialized callbacks
now become no-ops instead of dereferencing missing drawing state.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-text-drawing-inputs.py
```
