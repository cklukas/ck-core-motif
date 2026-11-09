#!/usr/bin/env python3
"""Check MWM discards stale leaders when WM_TRANSIENT_FOR changes."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmWinInfo.c").read_text(
    encoding="utf-8"
)
cycle_start = source.index("static Boolean\nWouldCreateTransientCycle")
cycle_end = source.index("/*\n * Global Variables:", cycle_start)
cycle = source[cycle_start:cycle_end]
assert "slow == client" in cycle
assert "fast == client" in cycle
assert "slow == fast" in cycle

start = source.index("void \nProcessWmTransientFor (ClientData *pCD)")
end = source.index("} /* END OF FUNCTION ProcessWmTransientFor */", start)
body = source[start:end]

assert "if (pCD == NULL)" in body
assert "pCD->transientFor = (Window)0L;" in body
assert "pCD->transientLeader = NULL;" in body

hint = body.index("#ifdef WSM")
assert body.index("pCD->transientFor = (Window)0L;") < hint
assert body.index("pCD->transientLeader = NULL;") < hint

resolve = body.index("!XFindContext")
assert "!WouldCreateTransientCycle(pCD, leader)" in body[resolve:]
assert body.index("pCD->transientFor = window;", resolve) > resolve
assert body.index("pCD->transientLeader = leader;", resolve) > resolve

group_start = source.index("if (flags & WindowGroupHint)")
group_end = source.index("#endif /* NO_OL_COMPAT */", group_start)
assert "!WouldCreateTransientCycle(pCD, leader)" in source[group_start:group_end]

print("PASS MWM resets stale and cyclic transient associations before linking")
