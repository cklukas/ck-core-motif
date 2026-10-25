#!/usr/bin/env python3
"""Check widget-name scratch allocation failure is handled."""
import os
from pathlib import Path
source=Path(os.environ.get("MOTIF_SOURCE",Path(__file__).parents[2]/"lib"/"Mrm"/"Mrmwcrw.c"))
text=source.read_text(encoding="utf-8")
start=text.index("UrmSetWidgetInstance ("); end=text.find("\n\n\f",start)
region=text[start:] if end<0 else text[start:end]
needle='''      c_name_tmp = (String)ALLOCATE_LOCAL((c_name_len + 2) * sizeof(char));\n      if (c_name_tmp == NULL)\n        return Urm__UT_Error("UrmSetWidgetInstance", _MrmMMsg_0040,\n                             NULL, context_id, MrmFAILURE);'''
if needle not in region: raise SystemExit("widget-name scratch allocation is unchecked")
print("Mrm widget-name allocation failure is handled")
