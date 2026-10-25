#!/usr/bin/env python3
"""Check CString-vector rebind failures are propagated after record growth."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmCWRSetArgCStringVec (")
end = text.find("\n\n\f", start)
region = text[start:] if end < 0 else text[start:end]
loop = region[region.index("for ( ndx=0 ; ndx<num_cstg ; ndx++ )"):]
needle = '''      widgetrec = (RGMWidgetRecordPtr) UrmRCBuffer(context_id) ;
      result = UrmCWR__BindArgPtrs
        (context_id, "UrmCWRSetArgCStringVec", arg_ndx, &argdesc, &argptr) ;
      if ( result != MrmSUCCESS ) return result ;'''
if needle not in loop:
    raise SystemExit("CString-vector rebind failure is not propagated")
print("PASS Mrm CString-vector rebind failures propagate")
