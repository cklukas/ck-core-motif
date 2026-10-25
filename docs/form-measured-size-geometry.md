# Form measured-size geometry

Form layout measures a child against the remaining width or height through
`Dimension *fwidth` and `Dimension *fheight`. Several attachment paths
subtracted offsets before converting the result to `int`; a small form could
therefore wrap the measured remainder and request an incorrect attachment.

All measured-size subtraction now converts the dimensions before arithmetic,
preserving normal attachment behavior while making narrow forms deterministic.
