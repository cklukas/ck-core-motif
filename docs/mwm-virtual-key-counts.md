# MWM virtual-key count handling

The HP virtual-key replacement scan now rejects negative table counts and
keeps the Xlib keyboard-map request count separate from the bounded unsigned
count used to walk the returned key map. This preserves the existing key
matching behavior without signed/unsigned wraparound at the scan boundary.
The server-reported keycode range is also checked before converting its lower
bound to Xlib's `KeyCode` type.

The source contract is covered by
`tests/automated/check-mwm-virtual-key-counts.py`.
