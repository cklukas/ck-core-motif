#!/usr/bin/env python3
"""Check XmString rendition construction validates input."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringFunc.c").read_text()
start = source.index("XmStringPutRendition(")
guard = source.index("if ((string == NULL) || (rendition == NULL))", start)
create = source.index("XmStringComponentCreate", guard)
assert "return(NULL);" in source[guard:create]
print("PASS XmString rendition input is validated")
