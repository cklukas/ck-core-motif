#!/usr/bin/env python3
"""Check that MWM restart-command construction is transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmXSMP.c").read_text(encoding="utf-8")
start = source.rindex("getNewRestartCmd(void)")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "SIZE_MAX / sizeof(char *)" in region
assert "XtCalloc((size_t)argc + 1" in region
assert region.count("if (newRestartCmd[i] == (char *)NULL)") >= 2
assert "goto failure;" in region
assert "freeNewRestartCmd(newRestartCmd);" in region
print("PASS MWM restart-command construction is transactional")
