---
nav_exclude: true
---
# DialogShell input-method geometry

DialogShell includes the input-method area in its shell-height negotiation.
That addition is now performed in a wide unsigned type and saturated before
assignment to Motif's legacy `Dimension` field.  The reverse adjustment to
the child height is guarded against underflow.  Ordinary input-method sizing
and shell negotiation are unchanged.
