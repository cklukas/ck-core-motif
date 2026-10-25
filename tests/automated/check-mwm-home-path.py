#!/usr/bin/env python3
"""Ensure MWM home-directory resolution does not overflow its fixed path buffer."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text(
    encoding="utf-8"
)
start = text.rindex("void GetHomeDirName(String  fileName)")
end = text.index("/*************************************<->*************************************", start + 1)
function = text[start:end]
assert "length = snprintf(fileName, MAXWMPATH + 1" in function
assert "length > MAXWMPATH" in function
assert "strcpy(fileName, ptr)" not in function
print("MWM home-directory resolution is bounded")
