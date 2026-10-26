#!/usr/bin/env python3
"""Check TabStack public navigation APIs reject null widget handles."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TabStack.c").read_text(encoding="utf-8")
select_start = source.index("XmTabStackSelectTab")
index_start = source.index("XmTabStackIndexToWidget")
selected_start = source.index("XmTabStackGetSelectedTab")
assert "if (widget == NULL)" in source[select_start:index_start]
assert "if (widget == NULL)" in source[index_start:selected_start]
assert "if (widget == NULL)" in source[selected_start:]
print("PASS TabStack navigation APIs validate null widgets")
