---
nav_exclude: true
---
# Sash cursor allocation

Sash instances share a per-display crosshair cursor.  When the X server cannot
allocate that cursor, Sash now leaves the cache empty, avoids registering a
destruction callback for an invalid cursor, and creates its window without the
`CWCursor` attribute.  X therefore retains the inherited cursor rather than
receiving an invalid cursor resource.

Later Sash instances can retry the normal shared-cursor allocation.  No public
resources or Sash behavior change when the allocation succeeds.
