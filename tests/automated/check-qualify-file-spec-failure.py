#!/usr/bin/env python3
"""Check deterministic failure handling in _XmOSQualifyFileSpec."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Xmos.c"
text = source.read_text(encoding="utf-8")
start = text.index("void\n_XmOSQualifyFileSpec(")
end = text.index("\n}\n", start) + 3
body = text[start:end]
assert "*pQualifiedDir = NULL;" in body
assert "*pQualifiedPattern = NULL;" in body
assert "if (fSpec == NULL)\n    return;" in body
assert "XtFree(fSpec);\n    return;" in body
assert "return NULL;" not in body
print("qualify-file-spec failure guard present")
