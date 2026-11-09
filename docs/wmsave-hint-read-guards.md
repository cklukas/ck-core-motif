---
nav_exclude: true
---
# WM save-hint property validation

`ProcessWmSaveHint` reads `_DT_WMSAVE_HINT`, whose payload is one 32-bit
save-hint flag word. X permits a property to have the expected type and
format while containing no items or a truncated suffix. The reader requests
and requires exactly one complete item before dereferencing the returned
buffer, and continues to clear the saved flags when the property is absent or
malformed.

This preserves the existing behavior for valid clients while preventing a
malformed empty, oversized, or truncated property from influencing `mwm`.
