#!/usr/bin/env python3
"""Check ScrollFrame navigator updates validate traits and list entries."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ScrollFrameT.c").read_text()
start = source.index("_XmSFUpdateNavigatorsValue(")
assert "if (sf == NULL)" in source[start:]
assert "if ((sf_trait == NULL) || (sf_trait->getInfo == NULL))" in source[start:]
assert "if (!inited || (nav_list == NULL)) return ;" in source[start:]
assert "if ((nav_trait == NULL) || (nav_trait->setValue == NULL))" in source[start:]
print("PASS ScrollFrame navigator update guards are present")
