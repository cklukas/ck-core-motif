#!/usr/bin/env python3
"""Check that MWM obtains the session-manager endpoint for WSM messages."""

from pathlib import Path

source = (
    Path(__file__).parents[2] / "clients" / "mwm" / "WmInitWs.c"
).read_text(encoding="utf-8")

start = source.index("wmGD.requestContextWin")
end = source.index("/* create topmost shell", start)
region = source[start:end]

assert "_DtGetSmWindow(DISPLAY," in region
assert "&wmGD.dtSmWindow" in region
assert "_DtGetMwmWindow(DISPLAY," not in region
assert "#include <Dt/SessionM.h>" in source

manage = (
    Path(__file__).parents[2] / "clients" / "mwm" / "WmManage.c"
).read_text(encoding="utf-8")
assert "SendClientMsg( wmGD.dtSmWindow," in manage
assert "wmGD.xa_DT_SM_WM_PROTOCOL" in manage

print("PASS MWM obtains the CDE session-manager window for WSM acknowledgements")
