#!/usr/bin/env python3
"""Check the merged modern implementation contract; native suites exercise behavior."""
from pathlib import Path
source = (Path(__file__).parents[2] / "lib/Xm/NumberBox.c").read_text()
s = "".join(source.split())
assert "xmTextFieldWidgetClass" in s
assert "xmArrowButtonWidgetClass" in s
assert "xmLabelWidgetClass" in s
assert "XCreateGC" not in s
print("PASS number-box-gc-setup")
