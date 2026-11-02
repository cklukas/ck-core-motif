#!/usr/bin/env python3
"""Keep virtual-binding conversion from dereferencing invalid Xt inputs."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "VirtKeys.c").read_text(
    encoding="utf-8"
)
start = source.index("CvtStringToVirtualBinding(Display")
end = source.index("static void\nFillBindingsFromDB", start)
body = source[start:end]

guard = body.index("if (dpy == NULL || fromVal == NULL || fromVal->addr == NULL ||")
assignment = body.index("str = (char *)fromVal->addr;", guard)
lookup = body.index("XtGetKeysymTable(dpy", assignment)

assert guard < assignment < lookup
assert "toVal == NULL" in body[guard:assignment]

print("PASS virtual-binding converter validates Xt inputs before use")
