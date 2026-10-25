#!/usr/bin/env python3
"""Check failed compressed resource-tag lookup stops before string use."""
import os
from pathlib import Path
source = Path(os.environ.get("MOTIF_SOURCE", Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"))
text = source.read_text(encoding="utf-8")
start = text.index("\nUrm__CW_CreateArglist (")
end = text.find("\n\n\f", start)
region = text[start:end]
needle = '''Urm__UT_Error ("Urm__CW_CreateArglist",
				     err_msg, NULL, NULL, uncmp_res) ;
		      continue;'''
if needle not in region:
    raise SystemExit("resource-tag lookup failure does not stop before strcmp")
print("Mrm resource-tag lookup failures stop before resource-name use")
