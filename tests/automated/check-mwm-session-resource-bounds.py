#!/usr/bin/env python3
"""Check that mwm session resource names are dynamically sized."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmXSMP.c").read_text()
start = source.index("static char *\ngetClientResource")
end = source.index("\n/*\n *  Assumes: pCD", start)
region = source[start:end]
assert "char resourceBuf[MAX_RESOURCE_LEN]" not in region
assert "sprintf(resourceBuf" not in region
assert "snprintf(NULL, 0, fmtStr, clientID)" in region
assert "XtFree(resourceBuf);" in region
start = source.index("static void\ndbRemoveProxyClientEntry")
end = source.index("\n/*\n *  Add callbacks", start)
region = source[start:end]
assert "char resourceBuf[MAX_RESOURCE_LEN]" not in region
assert "strcat(resourceBuf" not in region
assert "resourceBuf[resourceLength] = ':'" in region
assert "XtFree(resourceBuf);" in region
print("mwm session resource names are dynamically bounded")
