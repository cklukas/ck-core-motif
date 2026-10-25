---
nav_exclude: true
---

# Avatar initials geometry bounds

`XmAvatar` now bounds initials text-width arithmetic before converting it to an
`int` for drawing. Normal initials retain their existing centered appearance;
extremely long values no longer wrap the calculated drawing position.

The focused regression check is
`tests/automated/check-avatar-initials-geometry-bounds.py`.
