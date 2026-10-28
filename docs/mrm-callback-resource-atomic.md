---
nav_exclude: true
---

# Mrm callback resource-reference state

`UrmCWRSetCallbackItemRes` now appends the routine and resource descriptor
before updating the callback item. If the resource append fails, the existing
callback fields remain unchanged; successful references retain their routine,
resource representation, and offset serialization.
