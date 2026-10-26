# MWM title drawing input guard

`DrawWindowTitle()` now ignores a null client record before reading decoration,
appearance, or title-window state.  Exposure callbacks can therefore safely
run during client teardown while valid title redraws retain their existing
alignment and font behavior.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-title-drawing-inputs.py
```
