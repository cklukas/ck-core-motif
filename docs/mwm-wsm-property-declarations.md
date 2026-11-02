---
nav_exclude: true
---
# MWM WSM property declarations

The WSM workspace-property writer directly uses Motif atom caching and the
window manager's XmString conversion helper. It now includes their declaring
headers instead of relying on incidental declarations from another build
configuration. This keeps strict C builds type-checked without changing the
property format or runtime behavior.
