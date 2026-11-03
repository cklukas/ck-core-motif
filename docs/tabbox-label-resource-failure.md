# TabBox label drawing resource failure

The horizontal and vertical `XmTabBox` label paths use temporary bitmaps to
render and rotate labels, and a second bitmap when a background image needs a
clip mask. Every temporary server resource is now checked before use.

If the label scratch bitmap or rotated image cannot be created, that label pass
ends safely. If only the optional clip-mask bitmap cannot be created, the label
is drawn directly instead. This retains normal rendering whenever resources are
available, avoids invalid X requests under server resource pressure, and does
not change the TabBox API or resource ownership.

The source-contract regression is
`tests/automated/check-tabbox-image-failures.py`.
