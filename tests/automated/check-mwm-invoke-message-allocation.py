#!/usr/bin/env python3
"""Check MWM invoke-message construction handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmCmd.c").read_text(
    encoding="utf-8"
)
start = source.index("void\nSendInvokeMessage")
end = source.index("/*----------------------------------------------------------------------*", start)
region = source[start:end]
allocation = "msg = fulldata = (MessageData)XtMalloc(sizeof(CARD8) * size);"
assert allocation in region
check = "if (fulldata == NULL)"
assert check in region
assert region.index(check) > region.index(allocation)
assert region.index("return;") > region.index(check)
assert region.index("return;") < region.index("msg = PackCARD32")
print("PASS MWM invoke-message allocation failure is handled")
