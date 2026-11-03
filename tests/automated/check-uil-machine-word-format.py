#!/usr/bin/env python3
"""Keep UIL's fixed-width 64-bit listing fields fully initialized."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "uil" / "UilLstLst.c").read_text(
    encoding="utf-8"
)
assert source.count('sprintf ((char *)hex_longword, "%016lX", (* code_ptr));') == 2
assert 'sprintf ((char *)hex_longword, "%lX", (* code_ptr));' not in source
print("UIL 64-bit machine-listing words are zero-padded to their copied width")
