#!/usr/bin/env python3
"""Check that XmPicture parser growth fails closed on allocation errors."""

from pathlib import Path

root = Path(__file__).parents[2]
source = (root / "lib/Xm/Picture.c").read_text(encoding="utf-8")
private = (root / "lib/Xm/PictureP.h").read_text(encoding="utf-8")

assert "allocation_failed" in private
assert "picture->allocation_failed || root_node == NULL" in source
assert "picture->nodes_alloced > INT_MAX / 2" in source
assert "picture->allocation_failed = True" in source
assert "if (new_node == NULL)" in source
assert "if (nodes == NULL)" in source
assert "if (newtrans == NULL) goto parse_failed;" in source
print("XmPicture allocation failures are bounded and fail closed")
