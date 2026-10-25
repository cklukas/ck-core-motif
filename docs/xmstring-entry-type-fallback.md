---
nav_exclude: true
---
# XmString entry-type fallback

Internal XmString entry helpers now treat null or unknown entry types as empty
data. Segment extraction stops safely for an unexpected component type. Valid
optimized, array, and unoptimized entries retain their existing behavior.

The regression check is
`tests/automated/check-xmstring-entry-type-fallback.py`.
