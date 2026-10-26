#!/usr/bin/env python3
"""Check bounded XmAvatar initials drawing geometry."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Avatar.c").read_text(encoding="utf-8")
set_start = source.rindex("static Boolean SetValues")
set_values = source[set_start:source.index("static XtGeometryResult QueryGeometry", set_start)]
assert "XmString replacement = copy_initials" in set_values
assert "if (replacement)" in set_values
assert "avatar->avatar.initials = old->avatar.initials" in set_values
assert "avatar->avatar.font = old->avatar.font" in set_values
print("PASS avatar resource replacements are transactional")
