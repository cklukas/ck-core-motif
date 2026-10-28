# TextOut width saturation

TextOut measures lines and margins in an Xt `Dimension`.  The resize path now
uses widened arithmetic and saturates at the representable Dimension maximum
when a very large document or metric would exceed it, instead of wrapping to a
small width.

Normal document measurements are unchanged.
