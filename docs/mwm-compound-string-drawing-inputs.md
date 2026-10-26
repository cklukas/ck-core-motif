# MWM compound-string drawing input guard

`WmDrawXmString()` now validates its display, font list, compound string,
graphics context, clip box, and active-screen state before calling
`XmStringWidth()` or the Motif drawing routines.  Valid multilingual title and
feedback strings retain their existing alignment, clipping, and clean-text
behavior.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-compound-string-drawing-inputs.py
```
