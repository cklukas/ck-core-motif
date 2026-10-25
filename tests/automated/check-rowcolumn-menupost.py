#!/usr/bin/env python3
"""Check that invalid menu-post mappings do not run success-only cleanup."""
from pathlib import Path
import sys

if len(sys.argv) != 2:
    raise SystemExit(f"usage: {sys.argv[0]} ROWCOLUMN_SOURCE")
text = Path(sys.argv[1]).read_text(encoding="utf-8")
start = text.index("if (_XmMapBtnEvent(RC_MenuPost(new_w)")
end = text.index("      }\n   }\n   else", start)
block = text[start:end]
assert "XmeWarning( (Widget) new_w, BadMenuPostMsg);" in block
failed = block.split("else", 1)[0]
succeeded = block.split("else", 1)[1]
assert "set_values_passive_grab(old, new_w);" not in failed
assert "XtFree(RC_MenuPost(old));" not in failed
assert "set_values_passive_grab(old, new_w);" in succeeded
assert "XtFree(RC_MenuPost(old));" in succeeded
print("PASS invalid menu-post mappings skip success-only grab and cleanup")
