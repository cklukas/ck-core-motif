#!/usr/bin/env python3
"""Check UTM send and queue allocations before dereferencing them."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWsmLib/utm_send.c"
text = source.read_text(encoding="utf-8")
send_start = text.index("UTMSendMessage(")
send_end = text.index("\n}\n", send_start) + 2
send_region = text[send_start:send_end]
queue_start = text.index("EnqueueUtmData(XtPointer data)")
queue_end = text.index("\n}\n", queue_start) + 2
queue_region = text[queue_start:queue_end]

assert "if (pUtmData == NULL)" in send_region
assert "if (!EnqueueUtmData(pUtmData))" in send_region
assert "XtFree((char *)pUtmData);" in send_region
assert "if (dataRec == NULL)" in queue_region
assert "return(False);" in queue_region
print("UTM send allocations are guarded")
