---
nav_exclude: true
---
# XmString context initialization guards

Context reinitialization now handles a null string without invoking optimized
representation access. Context copying also rejects a nonzero rendition count
with a null rendition array before copying it.

The regression check is
`tests/automated/check-xmstring-context-init.py`.
