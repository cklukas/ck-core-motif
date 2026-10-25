#!/usr/bin/env python3
"""Check argument-list resource values reject null descriptors."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nUrm__CW_CreateArglist (")
end = text.find("\n\n\f", start)
region = text[start:end]
needle = '''\t  resptr = (RGMResourceDescPtr) val ;
\t  if ( resptr == NULL )
\t    continue ;'''
if needle not in region:
    raise SystemExit("resource descriptor null guard is missing")
print("PASS Mrm argument-list resource values guard null descriptors")
