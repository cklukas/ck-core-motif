---
nav_exclude: true
---
# ScrolledWindow position bounds

ScrolledWindow computes scrollbar and viewport positions from widget dimensions
and padding, then stores them in X11's signed `Position` type.  Very wide
allocations can exceed that range; direct casts wrap the scrollbar or clip
origin to the opposite side of the window.

Scrollbar positions, all placement-policy branches, and the bounded viewport
helper now use a saturating conversion.  Ordinary layouts remain unchanged,
while extreme dimensions clamp to the representable X11 range.  The contract is protected by
`tests/automated/check-scrolled-position-bounds.py`.
