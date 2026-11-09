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

# CDE sends exactly one ready notification after initialization is complete.
# A legacy PANACOMM block used to send a second notification immediately after
# the lookup, before MWM had created its internal shell and message endpoint.
assert "#ifdef PANACOMM" not in region
ready = source.index("dtReadyNotification();")
wait_done = source.index("ShowWaitState (FALSE);")
assert ready > wait_done
assert source.count("dtReadyNotification();") == 1

manage = (
    Path(__file__).parents[2] / "clients" / "mwm" / "WmManage.c"
).read_text(encoding="utf-8")
assert "SendClientMsg( wmGD.dtSmWindow," in manage
assert "wmGD.xa_DT_SM_WM_PROTOCOL" in manage

print("PASS MWM uses CDE's session-manager endpoint and one final ready notification")
