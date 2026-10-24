#!/usr/bin/env python3
"""Check that WSM temporary config cleanup avoids shell command buffers."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text()
start = source.index("void\nDeleteTempConfigFileIfAny")
end = source.index("\n}\n\n", start)
region = source[start:end]
assert "char pchCmd[MAXWMPATH+1]" not in region
assert "/bin/rm" not in region
assert region.count("unlink(") == 2
start = source.index("static void ConfigStackPop (void)\n{")
end = source.index("\n}\n\n", start)
region = source[start:end]
assert "char pchCmd[MAXWMPATH+1]" not in region
assert "/bin/rm" not in region
assert "unlink(pConfigStackTop->cppName)" in region
print("mwm temporary config cleanup uses direct unlink")
