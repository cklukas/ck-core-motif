# XmScreen lookup bounds

When `XmGetXmScreen` creates an implied screen object, it searches the X11
display’s screen array. The previous loop called `ScreenOfDisplay` in its
increment expression before testing the loop bound, causing one lookup at
`ScreenCount(display)`. The loop now checks the bound first and performs each
lookup inside the body, preserving screen naming and creation behavior without
an out-of-range read.

`tests/automated/check-screen-lookup-bounds.py` guards this loop contract.
