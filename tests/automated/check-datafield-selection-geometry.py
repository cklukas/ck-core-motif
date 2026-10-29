#!/usr/bin/env python3
"""Check DataField selection and geometry APIs validate public inputs."""

import re
from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text(
    encoding="utf-8"
)

for name in (
    "XmDataFielddf_ClearSelection(",
    "XmDataFieldSetSelection(",
    "XmDataFieldXYToPos(",
    "XmDataFieldPosToXY(",
    "XmDataFieldShowPosition(",
    "XmDataFieldSetHighlight(",
):
    match = re.search(rf"(?m)^{re.escape(name[:-1])}\(", source)
    assert match is not None, name
    start = match.start()
    end = source.index("\n}\n", start)
    region = source[start:end]
    assert "if (w == NULL" in region, name
    assert region.index("if (w == NULL") < region.index(
        "_XmWidgetToAppContext(w)"
    ), name

position_match = re.search(r"(?m)^XmDataFieldPosToXY\(", source)
assert position_match is not None
position_start = position_match.start()
position_end = source.index("\n}\n", position_start)
assert "x == NULL || y == NULL" in source[position_start:position_end]

print("PASS DataField selection and geometry APIs validate inputs")
