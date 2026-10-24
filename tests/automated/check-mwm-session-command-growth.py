#!/usr/bin/env python3
"""Check WSM session-command pointer growth before insertion."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text()
start = source.index("void ParseSessionCommand")
end = source.index("} /* END OF FUNCTION ParseSessionCommand */", start)
region = source[start:end]
assert "if (argc == iSizeArgv)" in region
assert region.index("if (argc == iSizeArgv)") < region.index("argv[argc++] = string;")
assert "if (argv == NULL)" in region
assert "newArgv = (unsigned char **)XtRealloc" in region
assert "XtFree((char *)argv);" in region
print("mwm session-command growth occurs before pointer insertion")
