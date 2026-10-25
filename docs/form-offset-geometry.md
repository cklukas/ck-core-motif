# Form fallback offset geometry

When Form computes a child attachment without a temporary layout width or
height, it subtracts the attachment offset from the Form’s widget dimension.
Those fallback calculations now convert the dimension to `int` before
subtraction, preserving negative out-of-bounds positions instead of wrapping
them through unsigned arithmetic.
