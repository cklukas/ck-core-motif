#!/usr/bin/env python3
"""Check that MWM client-list growth preserves state on failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmXSMP.c").read_text(encoding="utf-8")
start = source.index("static Boolean\naddClientToList")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "if (cdList == NULL || nClients == NULL" in region
assert "SIZE_MAX / sizeof(*newPtr)" in region
assert "XtRealloc((char *)*cdList" in region
assert "if (*cdList != (ClientData **)NULL)" not in region
assert "(*nClients)++;" in region
print("PASS MWM client-list growth is transactional")
