---
nav_exclude: true
---
# FontSelector XLFD-mode callback guards

The XLFD-mode callback now rejects a missing selector or toggle callback record
before touching sibling toggles or changing modes. Valid selections retain the
existing mode transition behavior.
