#!/usr/bin/env python3
from pathlib import Path
import re
text=(Path(__file__).parents[2]/"lib"/"Xm"/"XmExtUtil.c").read_text()
for name in ("_XmMoveWidget", "_XmResizeWidget", "_XmConfigureWidget"):
    match=re.search(r"\b"+re.escape(name)+r"\s*\([^)]*\)\s*\{(.*?)\n\}", text, re.S)
    assert match and "if (w == NULL)" in match.group(1), name
print("Xm geometry wrappers guard null widgets")
