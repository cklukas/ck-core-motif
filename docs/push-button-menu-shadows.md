---
nav_exclude: true
---
# PushButton menu shadow bounds

The PushButton menu border-highlight and enter callbacks now draw shadows only
when the highlight frame leaves a positive interior and both shadow GCs exist.
The core dimensions are converted before subtraction, so undersized or
partially initialized menu entries cannot pass invalid rectangles to X11.

Normal-sized menu entries retain their existing shadow type and callback order.
The focused regression is
`tests/automated/check-push-button-menu-shadows.py`.
