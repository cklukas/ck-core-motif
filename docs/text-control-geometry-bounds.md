---
nav_exclude: true
---
# Text-control preferred geometry bounds

Prompt, Link, and Badge preferred-width calculations now perform text-length
multiplication in a wide type and clamp to the platform `Dimension` maximum.
Long labels no longer wrap into unexpectedly narrow geometry requests; normal
labels retain their existing sizing.

`tests/automated/check-text-control-geometry-bounds.py` protects these paths.
