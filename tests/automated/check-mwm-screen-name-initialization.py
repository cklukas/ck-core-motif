#!/usr/bin/env python3
"""Check WSM screen-name initialization does not write through an unallocated buffer."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmInitWs.c").read_text(
    encoding="utf-8"
)
start = source.index("void InitScreenNames (void)")
init_region = source[start:]
create_start = source.index("InitWmScreen (WmScreenData *pSD, int sNum)")
region = source[create_start:source.index("void InitScreenNames (void)", create_start)]
assert 'char screenName[sizeof("-2147483648")]' in region
assert 'snprintf(screenName, sizeof(screenName), "%d", sNum)' in region
assert 'memcpy((char *)wmGD.screenNames[sNum], screenName, buf_size)' in region
assert 'sprintf (buffer, "%d", sNum)' not in region
init = source[source.index("void InitScreenNames (void)"):]
assert 'snprintf((char *)wmGD.screenNames[num], 4' in init
assert "nameLength < 0 || nameLength >= 4" in init
assert 'sprintf((char *)wmGD.screenNames[num]' not in init
print("PASS WSM screen names use an initialized bounded buffer")
