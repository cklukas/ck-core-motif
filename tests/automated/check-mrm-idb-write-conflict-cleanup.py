#!/usr/bin/env python3
"""Check Mrm IDB write-open conflict paths close existing descriptors."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIfile.c").read_text(encoding="utf-8")
start = source.index("if (access == URMWriteAccess)")
end = source.index("/* Else this file is to opened for read access */", start)
region = source[start:end]
for condition in ("if (os_ext == 0)", "else if (!os_ext->nam_flg.clobber_flg)", "else if (os_ext->version != MrmOsOpenParamVersion)"):
    pos = region.index(condition)
    close = region.index("close (file_desc)", pos)
    assert close < region.index("return", pos), condition
print("Mrm IDB write-open conflict paths close existing descriptors")
