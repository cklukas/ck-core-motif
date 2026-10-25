#!/usr/bin/env python3
"""Check XmStringPutRendition validates its string and rendition inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringFunc.c").read_text()
start = source.index("XmStringPutRendition(")
guard = source.index("if ((string == NULL) || (rendition == NULL))", start)
strlen_call = source.index("strlen(rendition)", guard)
assert guard < strlen_call
assert "return(NULL);" in source[guard:strlen_call]
print("PASS XmString rendition input validation is present")
