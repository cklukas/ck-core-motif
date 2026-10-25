# MenuShell popup placement bounds

MenuShell popup edge placement now computes screen clamping in signed
arithmetic.  Oversized or temporarily collapsed menus no longer wrap through
unsigned `Position` and `Dimension` subtraction when moved back on screen.

The source-level regression is `tests/automated/check-menushell-placement.py`.
