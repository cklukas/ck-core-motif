#!/usr/bin/env python3
"""Check that XmOS filename and home-directory allocations are guarded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xmos.c").read_text(encoding="utf-8")
home = source.index("XmeGetHomeDirName(void)")
home_end = source.index("#ifndef LIBDIR", home)
home_region = source[home:home_end]
assert "homeDir = XtMalloc" in home_region
assert "if (homeDir != NULL)" in home_region
name = source.index("_XmOSBuildFileName(String path")
name_end = source.index("/************************************************************", name)
name_region = source[name:name_end]
assert name_region.count("if (fileName == NULL)") == 2
print("PASS XmOS filename allocations are guarded")
