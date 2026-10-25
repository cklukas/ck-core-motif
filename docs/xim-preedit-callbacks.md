---
nav_exclude: true
---
# XIM preedit callback guards

The preedit-start and preedit-done callbacks now reject missing XIM client data
before looking up widget state. This makes malformed or late external input
method callbacks harmless while preserving normal callback dispatch.

The regression check is
`tests/automated/check-xim-preedit-callbacks.py`.
