#!/usr/bin/env python3
"""Keep MWM's ToolTalk network-file fallback on its owned split string."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text(
    encoding="utf-8"
)
start = source.index("static String\nGetNetworkFileName")
body = source[start : source.index("/****************************   eof", start)]
fallback = body[body.index("shellscan had a problem") : body.index("/* convert to canonical host/file name */")]

assert "sName = XtNewString ((String) pchFile);" in fallback
assert "*pch = '\\0';" in fallback
assert "host_part = sName;" in fallback
assert "host_part = pchFile;" not in fallback
assert "file_part = pch+1;" in fallback

print("MWM network-file fallback passes the owned split host to ToolTalk")
