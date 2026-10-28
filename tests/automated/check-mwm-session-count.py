#!/usr/bin/env python3
"""Check that MWM validates the session item count before allocation."""
from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "clients" / "mwm" / "WmResParse.c").read_text()
start = source.index("void\nParseSessionItems")
end = source.index("\n} /* END OF FUNCTION ParseSessionItems", start)
region = source[start:end]

assert "parsed = strtol((char *)line, &end, 10);" in region
assert "errno == ERANGE" in region
assert "parsed < 1 || parsed > INT_MAX" in region
assert "*end != '\\0'" in region
assert 'Warning("Invalid session item count");' in region
assert "totalSessionItems = atoi" not in region
print("MWM session-count contract passed")
