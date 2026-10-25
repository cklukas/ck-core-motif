#!/usr/bin/env python3
"""Check RowColumn menu-post replacement preserves state on copy failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RowColumn.c").read_text()
start = source.index("if (RC_MenuPost(new_w) != RC_MenuPost(old))")
end = source.index("\n   else   /* For backwards compatibility", start)
region = source[start:end]

assert "String menu_post_replacement;" in source
assert "menu_post_replacement = RC_MenuPost(new_w)" in region
assert "if (RC_MenuPost(new_w) && menu_post_replacement == NULL)" in region
assert "RC_MenuPost(new_w) = RC_MenuPost(old);" in region
assert "RC_PostEventType(new_w) = RC_PostEventType(old);" in region
assert "RC_PostButton(new_w) = RC_PostButton(old);" in region
assert "RC_PostModifiers(new_w) = RC_PostModifiers(old);" in region
assert region.index("XtNewString(RC_MenuPost(new_w))") < region.index("XtFree(RC_MenuPost(old))")
print("RowColumn menu-post preserves state on allocation failure")
