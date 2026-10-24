#!/usr/bin/env python3
"""Check that UIL listing titles cannot overrun their fixed output buffer."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSarMod.c").read_text()
assert source.count("snprintf(Uil_lst_c_title2, UIL_LISTING_TITLE_SIZE") == 2
assert "sprintf(Uil_lst_c_title2" not in source
print("UIL listing title formatting is bounded")
