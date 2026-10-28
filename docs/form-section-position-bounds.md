---
nav_exclude: true
---
# FormSection position bounds

`XmFormSection` positions its header, content, and optional footer from
margin, spacing, and child dimensions. Directly narrowing those sums to
Motif's signed `Position` type could wrap a large layout.

The layout now clamps each origin to the representable `Position` range.
Normal sections retain their existing geometry, while extreme resource or
child sizes cannot place a child at an unrelated coordinate.

The focused source regression is
`tests/automated/check-form-section-position-bounds.py`.
