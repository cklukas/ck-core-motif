# Xm geometry helper null-safety

The private geometry helpers in `XmExtUtil.c` are used by composite widgets while
negotiating preferred sizes.  `_XmRequestNewSize` now rejects a null widget and
only writes negotiated dimensions when the corresponding output pointer is
provided.  `_XmHWQuery` rejects a null widget or preferred geometry record before
reading either object.  Invalid helper inputs return `XtGeometryNo` without
calling Xt or dereferencing caller-owned memory; valid calls retain the existing
geometry negotiation behavior.

These checks keep malformed or partially initialized widget requests from
crashing the process while preserving the existing Motif ABI and result values
for valid callers.
