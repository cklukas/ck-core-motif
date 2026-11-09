#!/usr/bin/env python3
"""Check MWM session client discovery validates public list inputs."""

from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmXSMP.c").read_text(
    encoding="utf-8"
)
start = source.index("GetSmClientIdClientList(ClientData ***clients, int *nClients)")
end = source.index("\n}\n\nvoid\nSortClientListByWorkspace", start)
body = source[start:end]

assert "if (clients == NULL || nClients == NULL)" in body
assert body.index("if (clients == NULL || nClients == NULL)") < body.index("*nClients = 0;")
assert "pCD = pCL->pCD;" in body
assert "if (pCD == NULL)" in body
assert body.index("if (pCD == NULL)") < body.index("pCD->smClientID")

print("PASS MWM session client discovery rejects invalid entries")
