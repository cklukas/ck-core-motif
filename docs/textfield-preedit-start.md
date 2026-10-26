# TextField preedit startup endpoints

`PreeditStart` now initializes both replacement endpoints to the current
cursor before querying the selection. If no selection is available, the
pending-delete path therefore performs a defined zero-length replacement
instead of passing uninitialized positions to the text engine.

The focused source contract is checked by
`tests/automated/check-textfield-preedit-start.py`.
