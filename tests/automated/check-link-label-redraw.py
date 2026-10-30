#!/usr/bin/env python3
"""Check Link reports redraw only after a label copy succeeds."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Link.c").read_text()
start = source.rindex("static Boolean SetValues")
end = source.index("static XtGeometryResult QueryGeometry", start)
body = source[start:end]
success = body.index("if (label_replacement)")
failure = body.index("} else", success)
assert "redraw = True;" in body[success:failure]
assert "redraw = True;" not in body[failure:body.index("}", failure) + 1]
assert "link->link.label = old->link.label;" in body

print("Link label copies report redraw only after success")
