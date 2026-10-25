#!/usr/bin/env python3
"""Check the simple TabList modify helper forwards its label argument."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TabList.c").read_text()
start = source.index("XmTabbedStackListSimpleModify(XmTabbedStackList tab_list")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "attributes.label_string = label_string;" in region
assert region.index("attributes.label_string = label_string;") < region.index("XmTabbedStackListModify")
print("PASS TabList simple modify forwarding")
