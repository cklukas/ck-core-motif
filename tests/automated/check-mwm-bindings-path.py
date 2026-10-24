#!/usr/bin/env python3
"""Check Linux Motif binding-path construction and lifetime handling."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text()
start = source.index("void ProcessMotifBindings")
end = source.index("} /* END OF FUNCTION ProcessMotifBindings */", start)
region = source[start:end]
assert "char           fileName[MAXWMPATH+1]" not in region.split("#else", 1)[0]
assert "strcpy (fileName, homeDir)" not in region
assert "fileNameLength = strlen(homeDir) + strlen(MOTIF_BINDINGS_FILE) + 2" in region
assert "snprintf(fileName, fileNameLength" in region
assert "XtFree (fileName);" in region
print("mwm Linux binding paths are dynamically bounded")
