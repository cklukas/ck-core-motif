# PushButton default-glyph bounds

Optional default-glyph drawing and erasure now validate the available right
margin and content height before placement.  Signed calculations prevent tiny
buttons or glyphs taller than the label area from wrapping into negative or
oversized X coordinates and dimensions.

The focused regression is `tests/automated/check-pushb-glyph-bounds.py`.
