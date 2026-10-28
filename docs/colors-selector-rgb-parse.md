---
nav_exclude: true
---

# ColorSelector RGB-file parsing

ColorSelector database lines now parse each decimal RGB channel with checked
`strtoul` conversion. Malformed, overflowing, or wider-than-16-bit channels
are rejected before they reach the color table; the existing color-name
handling remains unchanged.

The focused regression check is
`tests/automated/check-color-rgb-parse.py`.
