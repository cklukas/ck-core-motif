#!/usr/bin/env python3
"""Property replacement owns the record; callers must not free it twice.

The native Periodic menu regression also exercises repeated clipboard copies.
"""
from pathlib import Path
import re
source = (Path(__file__).parents[2] / "lib/Xm/CutPaste.c").read_text()
start = source.index('static int \nClipboardSendMessage(')
end = source.index('static int', start + len('static int \nClipboardSendMessage('))
body = source[start:end]
calls = list(re.finditer(r"ClipboardReplaceItem\s*\([^;]+;", body))
consuming = [c for c in calls if re.search(r"\broot_clipboard_header\b", c[0]) and "True" in c[0]]
assert len(consuming) == 1
following = body[consuming[0].end():]
following = following[:following.index("case XM_DATA_DELETE_MESSAGE:")]
assert not re.search(r"XtFree\(\s*\(char \*\)\s*root_clipboard_header\s*\)", following)
