# Drag-target compatibility input contract

`XmTargetsAreCompatible` compares two X11 target lists for a display.  It now
returns `False` when the display is null or when a non-empty target count has
no corresponding array, before application-context setup or array access.
Empty lists remain valid, and comparisons with valid inputs are unchanged.
