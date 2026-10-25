#!/usr/bin/env python3
"""Check WSM send failure paths release state and tolerate null callbacks."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWsmLib/send.c"
text = source.read_text(encoding="utf-8")
start = text.index("WSMSendMessage(")
end = text.index("\n}\n", start) + 2
region = text[start:end]

assert region.index("XtIsRealized(w)") < region.index("_WSMPackRequest")
assert "if (reply_callback != NULL)" in region
assert "if (req_info == NULL)" in region
assert "XtFree((XtPointer) msg_data);" in region
print("WSM send failure paths are guarded")
