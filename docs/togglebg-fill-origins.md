# ToggleButton fill origin bounds

The ToggleButton label and etched-menu fill paths derive their rectangle from
the gadget position plus highlight and shadow margins. Those resources are
unsigned `Dimension` values, while the position is a signed X11 coordinate.
The old locals could wrap the margin sum or the origin before the existing
width and height checks ran.

Both helpers now use signed `int` locals for the margin and fill origins. This
keeps off-edge and narrow gadgets deterministic without changing resources,
callbacks, the Motif ABI, or the fill dimensions. The source regression
`tests/automated/check-togglebg-fill-origins.py` covers both paths.
