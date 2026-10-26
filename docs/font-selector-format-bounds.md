---
nav_exclude: true
---
# Font-selector formatting bounds

Font selector resource fields and encoding-menu button names are written into
fixed local buffers. They now use snprintf, keeping formatting bounded even
when unusual font metrics or a large encoding index are supplied.

The source contract is covered by
tests/automated/check-font-selector-format-bounds.py.
