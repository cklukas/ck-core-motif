---
nav_exclude: true
---
# Mwm session resource-name bounds

Session-management resource names are built from client IDs and, for workspace
sessions, workspace names.  The legacy implementation formatted those names
into a fixed 1024-byte buffer in both resource lookup and proxy-entry removal.
Long IDs or workspace names could therefore overwrite stack storage.

Both paths now measure and allocate the complete resource name, release the
temporary buffer after the Xrm operation, and preserve the existing resource
keys and database update behavior.

`tests/automated/check-mwm-session-resource-bounds.py` protects this contract.
