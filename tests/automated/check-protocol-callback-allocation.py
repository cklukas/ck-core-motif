#!/usr/bin/env python3
"""Check that protocol callback registration handles entry allocation failure."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Protocols.c"
text = source.read_text(encoding="utf-8")
needle = """    if (protocol == NULL) {
\t_XmAppUnlock(app);
\treturn;
    }

    _XmAddCallback((InternalCallbackList *) &(protocol->protocol.callbacks),
"""
if needle not in text:
    raise SystemExit("protocol callback allocation guard is missing")

print("protocol callback allocation guard present")
