# MWM active icon attributes

Activating the optional icon label copies the icon frame's event mask to the
label window. The show path now checks `XGetWindowAttributes` before reading
that mask, so an icon frame destroyed during activation causes a clean return
instead of an invalid attribute update.
