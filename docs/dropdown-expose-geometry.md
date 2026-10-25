---
nav_exclude: true
---
# DropDown expose geometry

The DropDown visual-style expose handler positions its shadow from a child
origin minus child border and manager shadow thickness.  Those origins are
signed `Position`s, but the handler stored the subtraction in an unsigned
`Dimension`; a child temporarily positioned near or before zero could wrap the
shadow origin.

The handler now performs the subtraction in signed arithmetic and stores the
result as a `Position`, preserving normal shadows while keeping collapsed or
transient layouts bounded.
