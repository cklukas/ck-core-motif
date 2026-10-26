# MWM compact system and minimize geometry

`BevelSystemButton()` now returns below five pixels, and
`BevelMinimizeButton()` returns below seven pixels.  These are the minimum
heights supported by their legacy glyphs; rejecting smaller values before the
switch avoids unsigned subtraction wrapping while preserving normal title-bar
rendering.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-compact-gadget-geometry.py
```
