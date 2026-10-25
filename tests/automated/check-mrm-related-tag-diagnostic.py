#!/usr/bin/env python3
"""Check related-tag lookup errors identify the related code and stop."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nUrm__CW_CreateArglist (")
end = text.find("\n\n\f", start)
region = text[start:end]
marker = "argptr->stg_or_relcode.related_code,\n\t       &args[*num_used].name)"
pos = region.index(marker)
block = region[pos:region.find("\n\n    }\t/* Loop end */", pos)]
if "_MrmMMsg_0062, argptr->stg_or_relcode.related_code" not in block:
    raise SystemExit("related-tag diagnostic uses the wrong code")
if "NULL, NULL, uncmp_res) ;\n\t\tcontinue;" not in block:
    raise SystemExit("related-tag failure does not stop the argument")
print("PASS Mrm related-tag failures identify and stop the failed argument")
