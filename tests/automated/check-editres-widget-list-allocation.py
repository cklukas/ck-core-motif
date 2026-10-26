#!/usr/bin/env python3
"""Check edit-resources widget-list allocation before hierarchy writes."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "EditresCom.c").read_text(
    encoding="utf-8"
)
start = source.index("widget_list = (unsigned long *)")
end = source.index("/*\n     * Put the widgets", start)
region = source[start:end]
assert "if (widget_list == NULL)" in region
assert "return;" in region
print("edit-resources widget-list allocation is checked before hierarchy writes")
