---
nav_exclude: true
---

# MWM font-warning allocation widths

When MWM cannot obtain the default appearance font, it formats a localized
diagnostic containing the font list before reporting the error. The formatted
length is now checked for negativity and `UINT_MAX` representability before
conversion to an Xt `Cardinal`. The normal diagnostic and fatal error path are
unchanged; only an unrepresentable formatting result uses the existing plain
fallback message.
