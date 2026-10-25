# CascadeButtonGadget cascade glyph guards

The option-menu cascade glyph subtracts shadow margins from its configured
width.  A small or incomplete gadget could underflow that unsigned arithmetic
and pass invalid dimensions or missing GCs to the X drawing helpers.

The glyph path now performs the subtraction in a signed temporary, rejects
non-positive results or insufficient height, and requires a realized parent
and all drawing GCs.  Normal cascade glyph rendering is unchanged for valid
geometry.
