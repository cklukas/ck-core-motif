---
nav_exclude: true
---
# Embedded XPM mask ownership

The XPM loader generated embedded mask names in a fixed 255-byte buffer and
ignored whether `_XmInstallImage` accepted the mask.  Long image names could
overflow the stack, while duplicate names or disabled mask use leaked the
loaded mask image.

The mask name is now dynamically sized and its size is checked against Xt's
`Cardinal` allocation interface. Failed installation, allocation failure, and
intentionally unused masks all destroy the temporary `XImage`; successful
installation transfers ownership to the image cache.

`tests/automated/check-embedded-mask-ownership.py` protects these rules.
