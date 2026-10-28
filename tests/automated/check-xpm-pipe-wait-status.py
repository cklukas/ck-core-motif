#!/usr/bin/env python3
"""Check XPM z-pipe setup validates waitpid before reading status."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmRdFToI.c").read_text(
    encoding="utf-8"
)
start = source.index("xpmPipeThrough(")
body = source[start : source.index("static int\nOpenReadFile", start)]
assert "pid_t waited;" in body
assert "waited = waitpid(pid, &status, 0);" in body
assert "if ( waited < 0 )" in body
assert "goto fail2;" in body
print("PASS XPM z-pipe validates waitpid status")
