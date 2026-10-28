---
nav_exclude: true
---

# Mrm legacy font-list marker

The 1.1 compatibility path now validates the legacy font-list validation
marker before copying entries into the current representation. Valid legacy
lists continue to be converted as before; malformed lists are rejected before
their offsets are dereferenced.
