# List geometry bounds

List traversal highlights and horizontal scrollbar sizing now calculate widths
and heights with signed intermediates before checking for non-positive values.
Collapsed lists therefore clamp or skip invalid geometry instead of allowing
unsigned subtraction to wrap into large draw or scrollbar dimensions.

The focused regression is `tests/automated/check-list-geometry-bounds.py`.
