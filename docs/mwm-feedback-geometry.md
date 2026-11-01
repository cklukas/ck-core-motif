# MWM feedback-window geometry

Move/resize feedback text uses signed X coordinates while the feedback window
dimensions are unsigned.  MWM now validates text-derived dimensions before
storing them, performs placement with signed 64-bit intermediates, and clamps
only at the Xlib coordinate boundary.  Text baselines use the same checked
path.  Negative geometry and feedback windows larger than the display therefore
no longer wrap through unsigned subtraction or overflow signed baseline
coordinates.

The focused source contract is covered by
`tests/automated/check-mwm-feedback-geometry.py`.
