#!/usr/bin/env python3
"""Check XmStringGetLtoR delegates locking to its public helpers."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringGet.c").read_text()
start = source.index("XmStringGetLtoR(")
end = source.index("  XmStringFreeContext (context);", start)
function = source[start:end]

assert "_XmProcessLock();" not in function
assert "_XmProcessUnlock();" not in function
assert "if (!XmStringInitContext (&context, string))" in function

print("XmStringGetLtoR avoids recursive process locking and handles init failure")
