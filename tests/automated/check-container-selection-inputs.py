#!/usr/bin/env python3
"""Check Container selection helpers validate inputs while walking nodes."""
from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)

select_start = source.rindex("CallSelectCB(\n")
select_region = source[select_start : source.index("\n}\n", select_start)]
assert "if (wid == NULL)" in select_region

selected_start = source.rindex("GetSelectedCwids(\n")
selected_region = source[selected_start : source.index("\n}\n", selected_start)]
assert selected_region.index("if (wid == NULL)") < selected_region.index("cw = (XmContainerWidget)wid")
assert selected_region.count("if (node->widget_ptr == NULL)") == 2
assert selected_region.count("if (c == NULL)") == 2

print("PASS Container selection helpers validate widget and constraint inputs")
