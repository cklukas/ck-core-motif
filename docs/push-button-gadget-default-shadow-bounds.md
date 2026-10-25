# PushButtonGadget default shadow bounds

`XmPushButtonGadget` uses either the external highlight thickness or the
internal 3D enhancement as the default-button shadow delta.  The previous
check only compared dimensions with the highlight thickness, so the internal
delta could exceed a small gadget and produce invalid subtraction results.

The draw and clear paths now reject rectangles no larger than the selected
delta before computing the inner shadow rectangle.  Valid default-button
shadows retain their existing colors and emphasis behavior.
