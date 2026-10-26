#!/usr/bin/env python3
"""Check SimpleMenu assembly tolerates failed widget creation."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Simple.c").read_text(
    encoding="utf-8"
)
start = source.index("EvaluateConvenienceStructure(")
start = source.index("static void \nEvaluateConvenienceStructure", start)
body_start = source.index("{", start)
body_end = source.index("\n}\n\nWidget", body_start)
body = source[body_start:body_end]
assert "if (wid == (Widget) NULL || sm == NULL)\n                return;" in body
assert body.count("child != (Widget) NULL && sm->callback") == 3
print("SimpleMenu assembly guards failed container and child creation")
