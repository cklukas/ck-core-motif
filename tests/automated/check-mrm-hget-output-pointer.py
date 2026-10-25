#!/usr/bin/env python3
"""Check Mrm hierarchy resource lookup validates its output pointer."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmhier.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmHGetIndexedResource (MrmHierarchy")
end = text.index("Urm__RegisterNamesInHierarchy (MrmHierarchy", start)
body = text[start:end]
needle = "if ( file_id_return == NULL )"
if needle not in body or body.index(needle) > body.index("*file_id_return ="):
    raise SystemExit("Mrm hierarchy resource lookup lacks output guard")
print("PASS Mrm hierarchy resource lookup validates its output pointer")
