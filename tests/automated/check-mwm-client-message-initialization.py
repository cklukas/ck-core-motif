#!/usr/bin/env python3
"""Check that MWM zeroes complete ClientMessage payloads before sending."""

from pathlib import Path


source = Path("clients/mwm/WmProtocol.c").read_text(encoding="utf-8")

for signature, next_marker in (
    ("void SendClientOffsetMessage (ClientData *pCD)", "void SendClientMsg"),
    ("void SendClientMsg (Window window", "Boolean AddWmTimer"),
):
    start = source.index(signature)
    end = source.index(next_marker, start)
    body = source[start:end]
    assert "XClientMessageEvent clientMsgEvent;" in body
    assert "memset(&clientMsgEvent, 0, sizeof(clientMsgEvent));" in body
    assert body.index("memset(&clientMsgEvent, 0, sizeof(clientMsgEvent));") < body.index(
        "clientMsgEvent.type = ClientMessage;"
    )
    assert body.index("memset(&clientMsgEvent, 0, sizeof(clientMsgEvent));") < body.index(
        "XSendEvent"
    )

assert "#include <string.h>" in source
print("PASS MWM client messages clear unused protocol payload words")
