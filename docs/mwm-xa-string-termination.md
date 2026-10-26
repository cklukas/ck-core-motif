# MWM XA_STRING property termination

X11 properties carry an item count; an XA_STRING value is not a promise of a
C-string terminator.  MWM now copies bindings and DT session-hint payloads into
Xt-owned buffers with one checked extra byte, writes the terminator explicitly,
and only then invokes the existing string parsers.  Empty, oversized, or
allocation-failed properties are rejected safely.
