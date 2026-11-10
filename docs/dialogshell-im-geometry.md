---
nav_exclude: true
---
# DialogShell input-method geometry

DialogShell includes the input-method area in its shell-height negotiation.
That addition is now performed in a wide unsigned type and saturated before
assignment to Motif's legacy `Dimension` field.  The reverse adjustment to
the child height is guarded against underflow.  Ordinary input-method sizing
and shell negotiation are unchanged.

Default dialog placement also calculates parent-centering and display-edge
adjustments in a wide signed type, then saturates only at the `Position`
output boundary.  Large virtual desktops and shell dimensions therefore do
not wrap a centered dialog to an unrelated location.

Coordinate translation now occurs only when the parent is realized.  An
unrealized parent retains its already-calculated centered coordinates instead
of reading the uninitialized translation outputs, so creating a dialog before
its parent maps has deterministic placement.
