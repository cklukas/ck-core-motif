#!/usr/bin/env python3
"""Check clipboard request messages release the updated header buffer."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("static int \nClipboardSendMessage(")
end = source.index("static int", start + 1)
body = source[start:end]

request = body[body.index("case XM_DATA_REQUEST_MESSAGE:"):body.index("case XM_DATA_DELETE_MESSAGE:")]
assert "ClipboardReplaceItem(display, XM_HEADER_ID" in request
assert "XtFree((char *)root_clipboard_header);" in request

print("Clipboard request messages release updated header buffers")
