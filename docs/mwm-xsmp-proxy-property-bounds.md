# MWM XSMP proxy-property bounds

MWM reads `WM_COMMAND` through an X text property while constructing proxy
client information.  The reader now rejects a missing or empty value before
accessing its first byte, and normalizes embedded NULs with an `i + 1 < nitems`
condition.  Empty properties therefore cannot turn unsigned count subtraction
into an out-of-bounds read.
