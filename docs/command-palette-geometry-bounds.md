# Command Palette geometry bounds

XmCommandPalette now saturates its preferred height before writing an X11
Dimension. Its list child also uses a representable signed Position, and the
remaining list height is clipped to the X11 signed-coordinate range.

Ordinary palette layouts are unchanged. At pathological geometry sizes, the
control retains a visible one-pixel list extent instead of wrapping a preferred
height or passing a negative child y-coordinate to Xt.

The focused source-contract check is:

    python3 tests/automated/check-command-palette-geometry-bounds.py
