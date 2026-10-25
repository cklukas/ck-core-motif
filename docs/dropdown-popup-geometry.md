# DropDown popup width geometry

`XmDropDown` computes the popup width when the list is opened.  The combo-box
width and popup offset use different signedness, so subtracting a large offset
from the `Dimension` directly could wrap to a near-maximum unsigned width.
The popup now performs the subtraction in a signed temporary and clamps the
minimum width to one pixel.  Normal offsets retain their existing width; an
offset that reaches past the combo box produces a small usable popup instead
of a giant geometry request.
