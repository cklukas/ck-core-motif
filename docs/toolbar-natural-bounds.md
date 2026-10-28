---
nav_exclude: true
---

# Toolbar natural width bounds

Toolbar natural-width accumulation now saturates at the representable
`Dimension` limit before narrowing. Large action sets can no longer wrap the
intermediate width and produce an unexpectedly tiny toolbar.
