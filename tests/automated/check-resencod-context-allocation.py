#!/usr/bin/env python3
"""Check compound-text conversion contexts guard initial allocations."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ResEncod.c").read_text()
public = source[source.index("XmCvtCTToXmString("):source.index("Hacked procedures")]
converter = source[source.index("cvtTextToXmString("):source.index("static char **")]
for body in (public, converter):
    assert "XtMalloc(sizeof(*ctx))" in body
    assert "if (ctx == NULL)" in body
    assert "ctx->dirstack == NULL" in body
    assert "XtFree((char *)ctx);" in body
print("PASS compound-text context allocations are guarded")
