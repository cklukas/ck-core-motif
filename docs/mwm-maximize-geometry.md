# MWM maximize-button compact geometry

`BevelMaximizeButton()` now returns for heights below five pixels.  Those
values are too small for the legacy maximize glyph, and the previous default
branch subtracted eight from an unsigned height before drawing, producing a
wrapped rectangle.  Normal title-bar heights retain the existing bevel
geometry.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-maximize-geometry.py
```
