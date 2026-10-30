#!/usr/bin/env python3
"""Check compound-text input conversion frees partial XmStrings on failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ResEncod.c").read_text()
start = source.index("XmString \nXmCvtCTToXmString(")
end = source.index("static char **\ncvtCTsegment(", start)
ct_to_xm = source[start:end]
assert "if (ok)\n      xmsReturned = (XmString)ctx->xmstring;" in ct_to_xm
assert "XmStringFree(ctx->xmstring);" in ct_to_xm

start = source.index("static Boolean \ncvtTextToXmString(")
end = source.index("static char **\ncvtCTsegment(", start)
text_to_xm = source[start:end]
assert "if (ok && ctx->xmstring != NULL)" in text_to_xm
assert "else\n      XmStringFree(ctx->xmstring);" in text_to_xm
print("PASS compound-text input failure cleanup")
