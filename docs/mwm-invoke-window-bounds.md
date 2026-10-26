# MWM invoke-command window bounds

The invoke-command protocol carries the invoking window as a 32-bit X11
identifier, while the local `Window` typedef can be wider.  `F_InvokeCommand`
now checks that the client window fits in `CARD32` before converting it and
rejects an unrepresentable value instead of silently truncating it.
