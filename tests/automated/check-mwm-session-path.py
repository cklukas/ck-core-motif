#!/usr/bin/env python3
"""Ensure WSM session-file path checks use size-safe bounded formatting."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmXSMP.c").read_text(
    encoding="utf-8"
)
start = text.index("buildDBFileName(char fileNameBuf[MAXPATHLEN]")
end = text.index("#ifndef WSM", start)
function = text[start:end]
assert "pathLength <= SIZE_MAX - fileLength - 2" in function
assert "snprintf(fileNameBuf, MAXPATHLEN" in function
assert "(int)strlen(savePath)" not in function
assert "sprintf(fileNameBuf" not in function
print("MWM WSM session-file path handling is bounded")
