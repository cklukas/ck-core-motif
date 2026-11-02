#!/usr/bin/env python3
"""Check MWM discards stale leaders when WM_TRANSIENT_FOR changes."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmWinInfo.c").read_text(
    encoding="utf-8"
)
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
assert body.index("pCD->transientFor = window;", resolve) > resolve
assert body.index("pCD->transientLeader = leader;", resolve) > resolve

print("PASS MWM resets stale transient associations before resolving property changes")
