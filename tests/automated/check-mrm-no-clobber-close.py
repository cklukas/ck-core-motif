#!/usr/bin/env python3
"""Check Mrm no-clobber open reports close failure instead of stale EXISTS."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIfile.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nIdb__FU_OpenFile (")
end = text.find("\n", start + 1)
region = text[start:] if end < 0 else text[start:end]
branch = region.index("else if (!os_ext->nam_flg.clobber_flg)")
close = region.index("if (close (file_desc) != 0)", branch)
exists = region.index("return MrmEXISTS", close)
if close > exists:
    raise SystemExit("Mrm no-clobber branch returns EXISTS before checking close")
print("Mrm no-clobber open handles existing-descriptor close failure")
