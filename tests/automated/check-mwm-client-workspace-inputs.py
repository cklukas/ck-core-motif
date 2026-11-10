#!/usr/bin/env python3
"""Check MWM session workspace aggregation guards its client input."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmXSMP.c").read_text(
    encoding="utf-8"
)
start = source.index("getClientWorkspaces(ClientData *pCD)")
end = source.index("\n}\n\n#endif /* WSM */", start)
body = source[start:end]

assert "WmScreenData *pSD;" in body
assert "WmScreenData *pSD = pCD->pSD;" not in body
guard = body.index("if (pCD == NULL)")
assignment = body.index("pSD = pCD->pSD;")
assert guard < assignment
assert body.index("if (pSD == NULL", assignment) > assignment

proxy_start = source.index("getProxyClientInfo(ClientData *pCD")
proxy_end = source.index("\n}\n\n/*", proxy_start)
proxy = source[proxy_start:proxy_end]
assert "if (pCD == NULL || pCD->pSD == NULL || proxyClientInfo == NULL)" in proxy

loader_start = source.index("LoadClientIconPositions(ClientData *pCD)")
loader_end = source.index("\n}\n#endif /* WSM */", loader_start)
loader = source[loader_start:loader_end]
assert "WmScreenData *pSD;" in loader
assert "WmScreenData *pSD = pCD->pSD;" not in loader
assert loader.index("if (pCD == NULL)") < loader.index("pSD = pCD->pSD;")
assert "pCD->pWsList == NULL" in loader

resource_start = source.index("getXSMPResource(ClientData *pCD")
resource_end = source.index("\n}\n\n/*", resource_start)
resource = source[resource_start:resource_end]
assert "if (pCD == NULL || pCD->smClientID == NULL || fmtStr == NULL)" in resource
print("PASS MWM session helpers guard incomplete client data")
