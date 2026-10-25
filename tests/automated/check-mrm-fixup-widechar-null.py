#!/usr/bin/env python3
"""Check wide-character fixup rejects a null serialized entry."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nUrm__CW_FixupValue (")
end = text.find("\n\n\f", start)
region = text[start:end]
needle = '''      wcharentry = (RGMWCharEntryPtr)val;
      if ( wcharentry == NULL )
\treturn MrmFAILURE ;'''
if needle not in region:
    raise SystemExit("wide-character fixup null guard is missing")
print("PASS Mrm wide-character fixup guards null entries")
