---
nav_exclude: true
---
# FontSelector bold callback guards

The bold callback now rejects missing toggle data and incomplete font/family
state before changing weight flags or rebuilding the displayed font. Valid
callbacks retain the existing bold/medium selection behavior.
