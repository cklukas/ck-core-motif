---
nav_exclude: true
---
# MWM hints read guards

`GetMwmHints` now accepts a property only when it has the expected 32-bit
format and at least the complete base Motif hints item count.  Truncated or
mis-typed client properties are released and treated as absent, preventing
callers from reading beyond the returned X property data.
