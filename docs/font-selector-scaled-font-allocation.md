# FontSelector scaled-font allocation

The scaled-font fallback in `XmFontSelector` now checks each temporary string
allocation before copying or formatting through it. Allocation failure reports
the existing unavailable-font error path instead of dereferencing a null
temporary buffer; successful font loading is unchanged.
