---
nav_exclude: true
---
# Image-scaling target screen

Scaled images are rendered into a temporary `XImage` before being copied to
the destination drawable. The temporary image now uses the visual of that
drawable's screen, discovered from its root window, instead of always using
the display default. Single-screen displays retain the fast default-screen
path.

The focused regression is
`tests/automated/check-image-scaling-screen.py`.
