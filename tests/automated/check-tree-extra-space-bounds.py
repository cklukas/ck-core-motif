#!/usr/bin/env python3
"""Check Tree extra-space arithmetic stays signed when content is larger."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Tree.c").read_text(encoding="utf-8")
assert "space = (int)w->core.height - (int)XmTreeC_bb_height(node);" in source
assert "space = (int)w->core.width - (int)XmTreeC_bb_width(node);" in source
assert "space = w->core.height - XmTreeC_bb_height(node);" not in source
assert "space = w->core.width - XmTreeC_bb_width(node);" not in source
print("tree extra-space arithmetic is signed")
