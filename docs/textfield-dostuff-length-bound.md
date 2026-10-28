# TextField locale-expanded transfer length bound

The drop-transfer path converts incoming compound or UTF-8 text to the
current locale before inserting it. In single-byte mode the converted byte
length is stored in the TextField selection record's `int` character count.
The implementation now checks the `strlen` result against `INT_MAX` before
the conversion, releases the transfer value, reports transfer failure, and
restores the insertion-point drawing state when the value is too large.

This keeps oversized locale-expanded transfers on the existing safe failure
path instead of allowing a `size_t` to `int` wrap. The source contract is
covered by `tests/automated/check-textfield-dostuff-length-bound.py`.
