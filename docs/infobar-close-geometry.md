# InfoBar close-icon geometry

InfoBar draws its close icon with fixed offsets from the right and bottom
edges.  The icon is now drawn only when the widget is large enough for those
offsets, and the coordinate arithmetic is explicitly signed.  Very small
InfoBars avoid wrapped X coordinates while normal close icons remain unchanged.
