#!/usr/bin/env python3
"""Check that XmTabList debug enum descriptions are bounded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmTabList.c").read_text()
start = source.index("#ifdef _XmDEBUG_XMTABLIST")
end = source.index("void \n_Xm_dump_tab", start)
region = source[start:end]
assert region.count("snprintf(buf, sizeof(buf)") == 3
assert "sprintf(buf" not in region
print("XmTabList debug enum formatting is bounded")
