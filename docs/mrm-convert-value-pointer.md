---
nav_exclude: true
---

# Mrm value-conversion destination

`Urm__CW_ConvertValue` now rejects a null value pointer before entering its
representation-specific conversion branches. This prevents immediate null
dereferences while leaving valid argument, vector, font, color, and pixmap
conversions unchanged.
