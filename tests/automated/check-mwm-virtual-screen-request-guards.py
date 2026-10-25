#!/usr/bin/env python3
"""Check that MWM virtual-screen requests validate payloads before unpacking."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmProtocol.c").read_text(encoding="utf-8")
start = source.index("static Boolean\nWMiConvert (")
end = source.index("    /*\n     * Handle the workspace manager protocol targets...", start)
region = source[start:end]

assert "inputFmt != WSM_PROTO_FMT || inputLen < 9" in region
assert "inputFmt != WSM_PROTO_FMT || inputLen < 8" in region
assert "UnpackCARD32(&input)" in region
print("PASS MWM virtual-screen requests validate payload bounds")
