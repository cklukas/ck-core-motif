#!/usr/bin/env python3
"""Check WSM get-state reply unpacking fails closed on allocation failure."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWsmLib/pack.c"
text = source.read_text(encoding="utf-8")
start = text.index("case WSM_GET_STATE:", text.index("_WSMUnpackReply"))
end = text.index("\n\t}\n\tbreak;", start)
region = text[start:end]

assert "if (reply->get_state.win_info_list == NULL)" in region
assert "reply->get_state.num_win_info_list = 0;" in region
assert "reply->get_state.allocated = True;" in region
print("WSM get-state unpacking handles allocation failure")
