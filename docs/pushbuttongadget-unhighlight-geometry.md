---
nav_exclude: true
---
# PushButtonGadget unhighlight geometry

When a PushButtonGadget loses focus, its menu and internal highlight paths
clear an inset border.  The clear operation now checks that the gadget is
larger than twice the inset before subtracting from its unsigned rectangle
dimensions.  Very small gadgets leave the border untouched instead of passing
wrapped dimensions to the X drawing helper.
