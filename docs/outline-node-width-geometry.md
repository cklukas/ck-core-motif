# Outline child-width geometry

Outline negotiates child widths from the parent width minus each child's
horizontal position. The available width is now calculated as a signed
integer and clamped to one pixel before it is used in Xt geometry requests.
Deeply indented or temporarily out-of-bounds children therefore receive a
small valid request instead of a wrapped near-maximum width.
