#!/usr/bin/env python3
"""Check Disclosure label updates commit only after copying succeeds."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Disclosure.c").read_text()
start = source.rindex("static Boolean SetValues")
end = source.index("static XtGeometryResult GeometryManager", start)
body = source[start:end]

assert "XmString replacement =" in body
assert "if (replacement)" in body
assert "w->disclosure.label = before->disclosure.label;" in body
assert body.index("XmStringFree(before->disclosure.label)") > body.index(
    "if (replacement)"
)
assert "return changed || label_changed;" in body

print("Disclosure SetValues label replacement is transactional")
