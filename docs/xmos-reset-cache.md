---
nav_exclude: true
---
# Xmos cache reset allocation

Resetting the file-selection directory cache now checks the path-buffer
allocation before copying the qualified directory. If setup cannot be
completed, the caller disables caching for that scan and continues directory
enumeration without dereferencing a null path buffer.

Successful cache resets preserve the existing path and entry-cache behavior.
