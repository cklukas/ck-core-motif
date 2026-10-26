#!/usr/bin/env python3
"""Check that MWM embedded-client property growth is transactional."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmManage.c").read_text(
    encoding="utf-8"
)
start = source.index("Boolean \nUpdateEmbeddedClientsProperty")
end = source.index("\n}", start) + 2
region = source[start:end]

assert "Window *resizedClients;" in region
assert "size_t allocation_bytes;" in region
assert "numClients == UINT_MAX" in region
assert "resizedClients = (Window *)XtRealloc" in region
assert "pClients = resizedClients;" in region
assert "pClients[numClients] = pECD->pCD->client;" in region
assert "if (rval)" in region
assert "pClients = (Window *) XtRealloc" not in region
print("PASS MWM embedded-client growth preserves state on allocation failure")
