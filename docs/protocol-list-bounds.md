---
nav_exclude: true
---
# Protocol list bounds

`XmAddProtocols` can receive more than the historical 32-protocol fast-path
size. Protocol property updates now use a stack buffer for small lists and an
owned buffer for larger lists. Larger lists are checked against Xt's `Cardinal`
byte limit before the owned allocation, so a representable protocol count
cannot narrow into a truncated heap size. Removal compacts the existing
protocol array without a fixed-size match table, and null protocol arrays are
rejected at the public entry points. Existing callback, property and ordering
behavior is unchanged.

The focused source check is
`tests/automated/check-protocol-list-bounds.py`.
