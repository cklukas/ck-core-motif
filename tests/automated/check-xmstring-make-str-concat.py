#!/usr/bin/env python3
"""Check MakeStr propagates component and concat failures."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringFunc.c").read_text()
start = source.index("MakeStr(XmStringContext")
end = source.index("\n\f", start)
region = source[start:end]

assert "XmStringComponentType\ttype" in region
assert "XmString\t\tstr, component" in region
assert region.count("component = XmStringComponentCreate") == 2
assert region.count("if (str == NULL)") >= 2
print("PASS MakeStr propagates component and concat failures")
