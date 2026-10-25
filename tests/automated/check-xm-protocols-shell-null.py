#!/usr/bin/env python3
from pathlib import Path
text=(Path(__file__).parents[2]/"lib"/"Xm"/"Protocols.c").read_text()
assert text.count("if (shell == NULL)") >= 7
for name in ("XmAddProtocols", "XmRemoveProtocols", "XmAddProtocolCallback", "XmRemoveProtocolCallback", "XmActivateProtocol", "XmDeactivateProtocol", "XmSetProtocolHooks"):
    assert name in text
print("Xm protocol APIs guard null shells")
