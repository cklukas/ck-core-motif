# MWM active icon geometry queries

The active icon label is positioned from the icon frame's current geometry.
Both show and move operations now require `XGetGeometry` to succeed before
using the returned coordinates. If the frame has disappeared or become
unqueryable, the optional label operation returns without changing state.
