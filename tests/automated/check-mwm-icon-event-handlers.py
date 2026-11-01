#!/usr/bin/env python3
"""Check MWM icon-box event handlers use Xt's callback ABI directly."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmIconBox.c").read_text(
    encoding="utf-8"
)
header = (Path(__file__).parents[2] / "clients/mwm/WmIconBox.h").read_text(
    encoding="utf-8"
)

for handler in (
    "UpdateIncrements",
    "HandleIconBoxButtonMotion",
    "HandleIconBoxIconKeyPress",
    "ChangeActiveIconboxIconText",
):
    assert f"(XtEventHandler){handler}" not in source
    assert handler in header

assert "void UpdateIncrements (Widget sWidget, XtPointer client_data, XEvent *event," in source
assert "void ChangeActiveIconboxIconText (Widget icon, XtPointer client_data," in source
assert "void HandleIconBoxIconKeyPress (Widget icon, XtPointer client_data," in source
assert "void HandleIconBoxButtonMotion (Widget icon, XtPointer client_data," in source
assert "Boolean *continue_to_dispatch" in source
assert "keyEvent = &event->xkey;" in source
assert "event->xfocus.send_event" in source
print("PASS MWM icon-box handlers use the Xt event-handler ABI")
