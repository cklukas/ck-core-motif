---
nav_exclude: true
---
# FontSelector italic callback guards

The italic callback now rejects missing toggle data and incomplete font/family
state. It also handles missing quark strings and bounds the slant copy to the
FontSelector field before rebuilding the displayed font.
