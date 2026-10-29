#!/usr/bin/env python3
"""Check the public DataField baseline getter validates its widget."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text(
    encoding="utf-8"
)
start = source.index("XmDataFieldGetBaseline(")
end = source.index("\n}\n", start)
region = source[start:end]
assert "if (w == NULL)" in region
assert "return 0;" in region
assert region.index("if (w == NULL)") < region.index(
    "_XmWidgetToAppContext(w)"
)
print("PASS DataField baseline getter validates NULL widgets")
