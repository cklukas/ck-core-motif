---
nav_exclude: true
---

# Mrm argument floating-point input

`UrmCWRSetArgValue` receives floating-point argument values by address. It now
rejects a null address before changing the argument descriptor, so a failed
write cannot leave a stale argument with a newly assigned representation type.

Valid `MrmRtypeFloat`, `MrmRtypeHorizontalFloat`, and
`MrmRtypeVerticalFloat` values retain the existing serialized format.
