# Mwm localized system-menu bounds

The window manager assembles its built-in system menu from localized command
strings.  The legacy implementation concatenated those strings into a fixed
2048-byte buffer, so long translations could overrun the stack.

The menu is now measured with `snprintf`, allocated to its exact rendered
length, and formatted with a bounded second pass.  Menu ordering and fallback
behavior are unchanged, and long translations are preserved instead of
truncated.

`tests/automated/check-mwm-system-menu-bounds.py` protects this contract.
