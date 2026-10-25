# CascadeButtonGadget shadow redisplay guards

`XmCascadeButtonGadget` draws its shadow into the realized manager window.
During manager realization, resource replacement, or narrow gadget geometry,
its parent window, shadow GCs, and rectangle can be incomplete.

The armed and option-menu shadow path now requires a realized parent, both
shadow GCs, non-zero shadow thickness, and geometry larger than the highlight
margins.  Valid gadget rendering and menu behavior remain unchanged.
