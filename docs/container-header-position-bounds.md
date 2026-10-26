# Container header position bounds

When a Container changes between outline and detail modes, header height is
now applied through the existing saturating signed-position helper. This
prevents large header dimensions from wrapping `core.y` during subtraction or
assignment.

The focused source contract is checked by
`tests/automated/check-container-header-position-bounds.py`.
