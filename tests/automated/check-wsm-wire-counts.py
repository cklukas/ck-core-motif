#!/usr/bin/env python3
"""Check that outbound WSM CARD16 list fields cannot silently wrap."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "pack.c").read_text(
    encoding="utf-8"
)
assert "#define WSM_MAX_LIST_ITEMS ((int)(CARD16)~(CARD16)0)" in source

request_start = source.index("_WSMPackRequest(")
request_end = source.index("} /* _WSMPackRequest */", request_start)
request = source[request_start:request_end]
assert "request->get_background.screen < 0" in request
assert "request->get_background.screen > WSM_MAX_LIST_ITEMS" in request
assert "PackCARD16(data, (CARD16)request->get_background.screen)" in request

string_start = source.index("PackString(MessageData data, String str)")
string_end = source.index("\n}\n", string_start) + 2
string = source[string_start:string_end]
assert "size_t i, len;" in string
assert "len > (size_t)WSM_MAX_LIST_ITEMS" in string
assert "PackListNum(data, (int)len)" in string

sizing_start = source.rindex("WinDataSizePacked(Display")
sizing_end = source.index("/************************************************************", sizing_start)
sizing = source[sizing_start:sizing_end]
assert "num > WSM_MAX_LIST_ITEMS" in sizing
assert "win_data->data_len > WSM_MAX_LIST_ITEMS" in sizing

print("PASS WSM CARD16 list counts and strings are bounded before packing")
