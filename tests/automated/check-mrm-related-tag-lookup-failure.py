#!/usr/bin/env python3
"""Check primary compressed-tag lookup failure skips stale-arg processing."""
import os
from pathlib import Path
source = Path(os.environ.get("MOTIF_SOURCE", Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"))
text = source.read_text(encoding="utf-8")
start = text.index("\nUrm__CW_CreateArglist (")
end = text.find("\n\n\f", start)
region = text[start:end]
needle = (
    'Urm__UT_Error ("Urm__CW_CreateArglist", err_msg, ' + "\n"
    + "\t\t\t     NULL, NULL, uncmp_res) ;\n\t      continue;"
)
if needle not in region:
    raise SystemExit("primary tag failure does not skip stale-arg processing")
print("Mrm primary tag lookup failures stop argument post-processing")
