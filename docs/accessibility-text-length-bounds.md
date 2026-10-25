---
nav_exclude: true
---

# Accessibility text-length bounds

The accessibility snapshot builder now bounds InfoBar, SearchBox, and Prompt
text lengths before storing them in signed metadata fields. Normal accessibility
text is unchanged; unusually long values no longer wrap the reported length.

The focused regression check is
`tests/automated/check-accessibility-text-length-bounds.py`.
