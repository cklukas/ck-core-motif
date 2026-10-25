# PushButtonGadget default-glyph bounds

Optional default-glyph drawing and erasure now validate the available right
margin and content height before placement.  Signed calculations prevent tiny
gadgets or tall glyphs from wrapping into invalid X coordinates and
dimensions.

The focused regression is `tests/automated/check-pushbg-glyph-bounds.py`.
