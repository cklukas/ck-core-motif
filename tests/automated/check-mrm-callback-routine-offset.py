#!/usr/bin/env python3
"""Check callback fixup rejects empty routine-name offsets."""
import os
from pathlib import Path
source=Path(os.environ.get("MOTIF_SOURCE",Path(__file__).parents[2]/"lib"/"Mrm"/"Mrmwcrw.c"))
text=source.read_text(encoding="utf-8")
start=text.rfind("\nUrm__CW_FixupCallback ("); end=text.find("\n\n\f",start)
region=text[start:end]
needle='''      if ( itmptr->cb_item.routine < RGMWidgetRecordHdrSize ||\n           itmptr->cb_item.routine >= ((RGMWidgetRecordPtr)bufptr)->size ||\n           memchr ((char *)bufptr + itmptr->cb_item.routine, '\\0',\n                   ((RGMWidgetRecordPtr)bufptr)->size -\n                   itmptr->cb_item.routine) == NULL )\n        return MrmFAILURE ;\n      rtn_name = (String) bufptr + itmptr->cb_item.routine ;'''
if needle not in region: raise SystemExit("callback routine offset guard is missing")
print("Mrm callback fixup rejects empty routine offsets")
