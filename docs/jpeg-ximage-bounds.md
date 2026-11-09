---
nav_exclude: true
---

# JPEG XImage bounds

The JPEG decoder now rejects decoded dimensions that cannot be represented by
the signed `XImage` fields before allocating or constructing an image.  It also
rejects an invalid root depth before converting it to Xlib's unsigned API.

Grayscale expansion uses libjpeg's unsigned `JDIMENSION` counters and a
pre-decrement index, avoiding a signed conversion or an underflow at the start
of a row.  Ordinary supported JPEGs retain the same output pixels and layout.
