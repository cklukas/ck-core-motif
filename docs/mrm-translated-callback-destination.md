---
nav_exclude: true
---

# Mrm translated callback destination

When an old-format Mrm callback descriptor is converted, callback records are
now materialized through the newly translated descriptor.  The previous path
used the original resource pointer as the destination, which could write using
the wrong legacy layout.  Current-format descriptors keep the same behavior
because their translated pointer is the original descriptor.
