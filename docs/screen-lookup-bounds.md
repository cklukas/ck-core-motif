# XmScreen lookup bounds

Motif’s screen-wide cursor and font-unit setters, as well as
`XmGetXmScreen`’s implied-screen search, walk the X11 display’s screen array.
The previous loops called `ScreenOfDisplay` in their increment expressions
before testing the loop bound, causing one lookup at `ScreenCount(display)`.
Each loop now checks the bound first and performs the lookup inside the body,
preserving behavior without an out-of-range read.

`tests/automated/check-screen-lookup-bounds.py` guards this loop contract.
