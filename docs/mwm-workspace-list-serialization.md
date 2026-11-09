---
nav_exclude: true
---
# MWM workspace-list serialization

WSM session saving serializes the workspaces inhabited by a client.  The
serializer now rejects absent client, screen, and workspace-list state before
dereferencing it; it then skips nameless workspace records, uses size_t for
accumulated lengths, and rejects wrapped growth before XtRealloc. Valid
workspace lists retain their ordering and space-separated representation.

`tests/automated/check-mwm-workspace-list-serialization.py` protects this
contract.
