#!/usr/bin/env python3
"""Check Tree connector gutters skip a failed private GC."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Tree.c").read_text(
    encoding="utf-8"
)
start = source.index("static void\nDrawNodeConnection(")
end = source.index("\nstatic Boolean\nRepaintChildConnections", start)
body = source[start:end]

create = "gc = XCreateGC(display, XtWindow(child), 0, NULL);"
assert create in body and "if (gc == NULL) return;" in body
assert body.index(create) < body.index("if (gc == NULL) return;") < body.index("XCopyGC")

print("PASS Tree connector gutters guard private GC allocation")
