---
nav_exclude: true
---
# MWM session database-name classification

When MWM increments a numeric session database suffix, it now casts each
filename byte to `unsigned char` before calling `isdigit`. This keeps the
character classification defined for filenames containing bytes above 127
while preserving the existing suffix behavior.
