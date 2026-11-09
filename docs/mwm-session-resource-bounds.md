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

The signed formatter result is first widened to `size_t` and checked against
the Xt allocation width before conversion, including the extra delimiter byte
used when removing a proxy-client entry.  This avoids a narrowing cast making
an oversized formatter result appear valid on 64-bit builds.

`tests/automated/check-mwm-session-resource-bounds.py` protects this contract.
