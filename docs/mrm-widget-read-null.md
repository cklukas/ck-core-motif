---
nav_exclude: true
---
# Mrm widget-read null-record guards

The hierarchy, indexed-widget, and RID-widget readers now reject a null
context buffer before the byte-swapped-record fallback reads its validation
field. Malformed or empty resource data returns the existing bad-widget-record
error instead of dereferencing a null pointer.
