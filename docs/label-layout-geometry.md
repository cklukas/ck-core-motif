# Label layout geometry

Label and LabelG center label and accelerator text by subtracting margins,
highlight/shadow insets, and text extents from unsigned toolkit dimensions.
The old expressions performed the subtraction before converting to `int`, so
small or temporarily collapsed labels could wrap to large positive offsets.
Accelerator X placement had the same mixed signed/unsigned problem.

The layout paths now convert each geometry term before subtraction. Normal
labels retain their existing positions while small labels and gadgets remain
stable during resize and realization.
