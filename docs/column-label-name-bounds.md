# Column label names

Column creates an internal label child by appending `_label` to each child
widget name.  The legacy path truncated names longer than 240 bytes before
adding the suffix, which could make distinct long names collide.

The generated name is now measured and allocated to its full length, then
released after widget creation.  Existing short names and the `_label` naming
contract are unchanged.

`tests/automated/check-column-label-name-bounds.py` protects this behavior.
