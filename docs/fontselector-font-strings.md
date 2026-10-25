---
nav_exclude: true
---
# FontSelector font-string construction

FontSelector builds XLFD strings into caller-provided buffers. The builder now
honors the supplied buffer size and always leaves a terminated string, so long
font family or encoding names are truncated safely instead of overflowing the
widget-owned buffer. Existing callers continue to use the same XLFD format.

The installed-library widget regression constructs and realizes a
`XmFontSelector` through the normal public API.
