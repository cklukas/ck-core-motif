# List clear geometry

List highlight clipping and empty-list cleanup derive rectangles by subtracting
insets from unsigned widget dimensions. A collapsed list could wrap the clip
height or clear width and ask X to process a much larger region than the
widget.

Both paths now calculate available spans in signed integers and skip empty
clear operations when no positive area remains. Normal highlight and cleanup
behavior is unchanged for usable list sizes.
