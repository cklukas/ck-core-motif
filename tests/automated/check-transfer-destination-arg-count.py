#!/usr/bin/env python3
"""Ensure drop destination Xt argument counts use Cardinal."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("if (tc -> selection == MOTIF_DROP)")
end = source.index("/* If this is a drop", start)
body = source[start:end]

assert "Cardinal i;" in body
assert "XtSetArg(args[i], XmNiccHandle" in body
assert "XtGetValues(ds->dragContext, args, i);" in body
assert "int i;" not in body

print("PASS drop destination argument count uses Cardinal")
