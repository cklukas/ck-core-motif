#!/usr/bin/env python3
from pathlib import Path
source = Path(__file__).parents[2] / "clients/mwm/WmProperty.c"
text = source.read_text()
for signature, count, pointer in (("SetEmbeddedClientsProperty", "cEmbeddedClients", "pEmbeddedClients"), ("SetWorkspacePresence", "cPresence", "pWsPresence")):
    start = text.index(signature, text.index("void "))
    body = text[start:text.index("XChangeProperty", start)]
    assert f"{count} > 0 && {pointer} == NULL" in body
    assert "return;" in body
print("property serializers validate counted buffers")
