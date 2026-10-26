# MWM system-modal screen size validation

Before creating the system-modal input window, MWM now validates that the
signed Xlib screen dimensions are positive and then converts them explicitly
to the unsigned X window-size type. Invalid geometry is rejected for that
screen instead of becoming a wrapped, oversized window.

The source contract is covered by
`tests/automated/check-mwm-system-modal-screen-size.py`.
