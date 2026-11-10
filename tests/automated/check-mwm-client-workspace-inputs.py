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
assert "if (pCD == NULL || pCD->pSD == NULL || proxyClientInfo == NULL ||" in proxy
assert "wmGD.display == NULL" in proxy

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

client_resource_start = source.index("getClientResource(char *clientID")
client_resource_end = source.index("\n}\n\n/*", client_resource_start)
client_resource = source[client_resource_start:client_resource_end]
assert "if (clientID == NULL || fmtStr == NULL ||" in client_resource
assert "wmGD.clientResourceDB == (XrmDatabase)NULL" in client_resource

xsmp_match_start = source.index("findXSMPClientDBMatch(ClientData *pCD")
xsmp_match_end = source.index("\n}\n\nstatic Boolean\ngetProxyClientInfo", xsmp_match_start)
xsmp_match = source[xsmp_match_start:xsmp_match_end]
assert xsmp_match.index("if (pCD == NULL)") < xsmp_match.index("pCD->smClientID")

proxy_match_start = source.index("findProxyClientDBMatch(ClientData *pCD")
proxy_match_end = source.index("\n}\n\n/*", proxy_match_start)
proxy_match = source[proxy_match_start:proxy_match_end]
assert proxy_match.index("if (pCD == NULL)") < proxy_match.index("pCD->smClientID")

public_match_start = source.index("FindClientDBMatch(ClientData *pCD")
public_match_end = source.index("\n}\n\nBoolean\nGetSmClientIdClientList", public_match_start)
public_match = source[public_match_start:public_match_end]
assert public_match.index("if (pCD == NULL)") < public_match.index("findXSMPClientDBMatch")
print("PASS MWM session helpers guard incomplete client data")
