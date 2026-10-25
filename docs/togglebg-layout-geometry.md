---
nav_exclude: true
---
# ToggleButtonGadget layout geometry

ToggleButtonGadget has the same undersized-control cases as the widget: pixmap
centering, accelerator placement, and label centering subtract unsigned
dimensions before storing signed positions. A collapsed gadget could therefore
wrap its content origin.

The affected paths now perform signed subtraction before assigning positions,
preserving normal menu and gadget layout while keeping tiny entries bounded.
