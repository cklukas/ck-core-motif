#!/usr/bin/env python3
"""Check XmImMbResetIC validates widget and output-pointer inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmIm.c").read_text(encoding="utf-8")
start = source.index("XmImMbResetIC(")
region = source[start:]
assert "if (w == NULL || mb == NULL)" in region
assert region.index("if (w == NULL || mb == NULL)") < region.index("_XmWidgetToAppContext(w)")
assert region.index("if (w == NULL || mb == NULL)") < region.index("*mb = NULL")
print("PASS XmImMbResetIC validates input arguments")
