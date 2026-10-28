---
nav_exclude: true
---
# MWM menu-command whitespace

The menu-command matcher now casts label and command bytes to `unsigned char`
before calling `isspace`. This keeps matching defined for high-bit bytes in
localized menu strings while preserving the existing whitespace rules.
