#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/VendorS.c").read_text()
start = source.index("InitializePrehook(")
end = source.index("ttp->post_delay", start)
region = source[start:end]
assert "XtMalloc (sizeof (XmToolTipConfigTraitRec))" in region
assert "if (ttp == NULL)" in region
assert "if (ttp == NULL)\n\t    return;" in region
print("VendorShell tooltip-trait allocation guard present")
