#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/AccessibleDBus.c").read_text()
start = source.index("static Boolean memo_resize")
end = source.index("static void memo_remove", start)
region = source[start:end]
assert "if (!table)" in region
assert "return False;" in region
assert "return True;" in region
assert region.count("if (!memo_resize") >= 2
assert "if (!record)" in region
assert "return NULL;" in region
print("Accessibility D-Bus memo allocation guards present")
