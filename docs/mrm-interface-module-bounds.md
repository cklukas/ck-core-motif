---
nav_exclude: true
---
# Mrm interface-module record bounds

`MrmFetchInterfaceModule` obtains its topmost-widget names from a serialized
UID resource. Before iterating that list it now validates the descriptor's
signed count and proves that the requested number of `RGMTopmostDesc` entries
fits in the loaded resource context. A truncated or malformed module returns
the existing `MrmBAD_IF_MODULE` result instead of reading beyond its record.

The focused source regression is
`tests/automated/check-mrm-interface-module-bounds.py`.
