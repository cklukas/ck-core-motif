# Mwm screen display bounds

Per-screen startup derives the display environment string from Xlib's display
name.  The legacy code copied that name into fixed 256-byte scratch buffers,
which could overflow for long hostnames or transport-specific display names.

`InitWmScreen` now parses a private dynamically sized copy and formats into a
buffer sized from the complete input.  The resulting per-screen
`DISPLAY=...` value and resource initialization behavior are unchanged for
normal displays, while long names no longer overwrite stack storage.

`tests/automated/check-mwm-screen-display-bounds.py` protects this contract.
