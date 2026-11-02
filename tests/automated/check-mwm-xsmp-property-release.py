#!/usr/bin/env python3
"""Check that MWM releases XSMP Xlib property buffers through XFree."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmXSMP.c").read_text(
    encoding="utf-8"
)

helper_start = source.index("static void\nFreeProxyClientInfoProperties")
helper_end = source.index("\n}\n", helper_start) + 2
helper = source[helper_start:helper_end]
assert "XFree(proxyClientInfo->wmCommand);" in helper
assert "XFree(proxyClientInfo->wmClientMachine);" in helper
assert "free(" not in source

info_start = source.index("static Boolean\ngetProxyClientInfo")
info_end = source.index("\n/*\n *  IMPORTANT", info_start)
info = source[info_start:info_end]
assert "XFree((char *)textProperty.value);" in info

save_start = source.index("static Boolean\nsaveProxyClient")
save_end = source.index("\nstatic void\ndbRemoveProxyClientEntry", save_start)
save = source[save_start:save_end]
assert save.count("FreeProxyClientInfoProperties(&proxyClientInfo);") == 2

print("PASS MWM XSMP property buffers use Xlib ownership cleanup")
