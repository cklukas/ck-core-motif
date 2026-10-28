---
nav_exclude: true
---
# MWM client-command character classification

MWM command parsing now casts command-stream bytes to `unsigned char` before
calling `isalnum`. This avoids signed-char undefined behavior for high-bit
localized menu text while preserving the accepted command-name grammar.
