#!/usr/bin/env python3
"""Check Char8-vector allocation rebind failures are propagated."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmCWRSetArgChar8Vec (")
end = text.find("\n\n\f", start)
region = text[start:] if end < 0 else text[start:end]
needle = '''  result = UrmCWR__BindArgPtrs
    (context_id, "UrmCWRSetArgChar8Vec", arg_ndx, &argdesc, &argptr) ;
  if ( result != MrmSUCCESS ) return result ;'''
if region.count(needle) < 2:
    raise SystemExit("Char8-vector allocation and growth rebinds are not both checked")
print("PASS Mrm Char8-vector rebind failures propagate")
