---
nav_exclude: true
---
# EditRes diagnostic bounds

EditRes reports conversion and protocol errors to the resource editor as
text. Every diagnostic is now built in storage sized from the rendered text.
Toolkit messages that carry `%s` parameters (`noColormap`) are expanded by a
function that only understands `%s` and `%%` and takes its parameters from the
parameter list the toolkit passed, so a message cannot ask for more arguments
than exist or for another conversion. Normal text and the EditRes protocol
remain unchanged.

`tests/automated/check-editres-error-bounds.py` protects this contract, and
`tests/automated/check-editres-wire-protocol.c` exercises the messages.
