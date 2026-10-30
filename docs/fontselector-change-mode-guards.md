---
nav_exclude: true
---
# FontSelector mode-transition guards

`ChangeMode` now rejects incomplete selector/font state, tolerates absent
sensitivity-list entries, and skips missing mode toggles. XLFD transitions also
stop safely if the temporary “any” string cannot be created. Fully constructed
selectors retain the existing mode-switch behavior.
