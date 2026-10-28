---
nav_exclude: true
---

# Mrm value-fixup swap state

`Urm__CW_FixupValue` now rejects a missing byte-swap state pointer before any
representation-specific conversion.  This protects scalar and vector paths
from dereferencing invalid state while leaving valid native and byte-swapped
resource conversion unchanged.
