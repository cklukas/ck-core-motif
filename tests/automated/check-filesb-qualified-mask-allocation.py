#!/usr/bin/env python3
"""Check FileSB qualified-mask allocation failure leaves safe output fields."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FileSB.c").read_text()
allocation = "qualifiedMask = XtMalloc( 1 + qDirLen + strlen( qualifiedPattern)) ;"
guard = "if(    qualifiedMask == NULL    )"
assert allocation in source
assert source.index(guard) > source.index(allocation)
assert "qualifiedSearchData->value = NULL ;" in source
assert "XtFree( qualifiedPattern) ;" in source[source.index(guard):]
print("PASS FileSB qualified-mask allocation failure is safe")
