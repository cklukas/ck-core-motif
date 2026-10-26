# TextField cursor geometry failures

`PaintCursor` now stops before changing the cursor GC when `GetXYFromPos`
cannot represent the caret position in the X11 coordinate range. This avoids
drawing a misleading caret at `(0, 0)` after a geometry or text-layout
conversion failure.

The focused source contract is checked by
`tests/automated/check-textfield-cursor-geometry.py`.
