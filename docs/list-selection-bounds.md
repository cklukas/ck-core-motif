---
nav_exclude: true
---
# List selection bounds

`XmListGetMatchPos` and `XmListGetSelectedPos` check result sizes before
allocating caller-visible arrays. The internal selection builders also reject
invalid counts before constructing cached selection arrays.

List keeps a one-based selected-position cache while selection updates are in
flight. Keyboard, API, and deselection consumers now share a bounds check before
converting a position to an internal zero-based item index, so a stale zero or
out-of-range entry is ignored rather than dereferencing outside `InternalList`.
Valid selection behavior and the public List API remain unchanged.

The source contract is checked by
`tests/automated/check-list-selection-internals.py` and
`tests/automated/check-list-selection-consumer-bounds.py`.
