---
nav_exclude: true
---

# EmptyState layout bounds

EmptyState accumulates vertical layout positions in a wide type and bounds
them when converting to X11 `Position` coordinates. Large child sizes or
margins can no longer wrap later controls to an incorrect location.
