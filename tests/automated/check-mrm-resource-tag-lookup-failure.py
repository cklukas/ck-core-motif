#!/usr/bin/env python3
"""Check failed compressed resource-tag lookup stops argument processing."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nUrm__CW_CreateArglist (")
end = text.find("\n\n\f", start)
region = text[start:end]
needle = (
    'Urm__UT_Error ("Urm__CW_CreateArglist",\n'
    '\t\t\t\t     err_msg, NULL, NULL, uncmp_res) ;\n'
    "\t\t      continue;"
)
if needle not in region:
    raise SystemExit("resource-tag lookup failure does not stop processing")
print("PASS Mrm resource-tag lookup failures stop argument processing")
