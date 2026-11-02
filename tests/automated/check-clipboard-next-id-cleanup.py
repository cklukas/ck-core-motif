#!/usr/bin/env python3
"""Property replacement owns the record; callers must not free it twice.

The native Periodic menu regression also exercises repeated clipboard copies.
"""
from pathlib import Path
import re
source = (Path(__file__).parents[2] / "lib/Xm/CutPaste.c").read_text()
start = source.index('static itemId\nClipboardGetNewItemId(')
end = source.index('static void \nClipboardSetAccess(', start + len('static itemId\nClipboardGetNewItemId('))
body = source[start:end]
calls = list(re.finditer(r"ClipboardReplaceItem\s*\([^;]+;", body))
consuming = [c for c in calls if re.search(r"\bpropertynumber\b", c[0]) and "True" in c[0]]
assert len(consuming) == 1
following = body[consuming[0].end():]
assert not re.search(r"XtFree\(\s*\(char \*\)\s*propertynumber\s*\)", following)
