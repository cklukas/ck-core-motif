#!/usr/bin/env python3
"""Check UTM sink failure removes only its queued package."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWsmLib/utm_send.c"
text = source.read_text(encoding="utf-8")

assert "static Boolean RemoveUtmData" in text
send_start = text.index("UTMSendMessage(")
send_end = text.index("\n}\n", send_start) + 2
send_region = text[send_start:send_end]
assert "if (RemoveUtmData((XtPointer)pUtmData))" in send_region
assert "XtFree((char *)pUtmData);" in send_region
remove_start = text.index("RemoveUtmData(XtPointer data)")
remove_end = text.index("\n}\n", remove_start) + 2
remove_region = text[remove_start:remove_end]
assert "dataQueueTail = ptr->prev;" in remove_region
print("UTM sink failure removes its queued package")
