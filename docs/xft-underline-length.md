---
nav_exclude: true
---
# Xft UTF-16 underline lengths

The compound-string underline matcher stores wide-character segment lengths
in bytes, while `XftTextExtents16` consumes a count of 16-bit characters.  The
Xft calls now convert the byte offsets to character counts before measuring
the prefix and underline text.  This prevents reads beyond the segment and
keeps underline placement correct for wide-character Xft strings.
