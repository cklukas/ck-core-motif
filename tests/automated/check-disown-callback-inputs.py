#!/usr/bin/env python3
"""Check that selection-disown teardown validates callback inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("DisownCallback(Widget w")
end = source.index("\n}\n", start) + 3
region = source[start:end]
assert "w == NULL" in region
assert "(Atom) client_data == None" in region
assert "XtDisplay(w)" in region
print("selection-disown callback validates widget and selection inputs")
