---
nav_exclude: true
---

# Chip size arithmetic

Chip natural-size accumulation now widens its non-negative metrics explicitly.
Pixmap geometry is narrowed back to layout coordinates only after limiting it
to the available widget extent, preventing signed/unsigned conversions from
altering the rendered icon layout.
