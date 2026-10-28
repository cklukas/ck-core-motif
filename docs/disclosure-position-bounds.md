---
nav_exclude: true
---
# Disclosure position bounds

`XmDisclosure` lays out its arrow, header, and optional content from margins,
child dimensions, and spacing. Direct narrowing of those sums to `Position`
could wrap extreme geometry.

The layout now clamps each derived origin to the representable signed
`Position` range. Normal disclosure sections retain their existing geometry,
while unusually large resources cannot place children at unrelated locations.

The focused source regression is
`tests/automated/check-disclosure-position-bounds.py`.
