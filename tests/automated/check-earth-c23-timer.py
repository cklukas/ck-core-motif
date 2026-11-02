#!/usr/bin/env python3
"""Check the Earth demo's C23 callback and zero-speed timer contracts."""

from pathlib import Path


source = (
    Path(__file__).resolve().parents[2]
    / "demos"
    / "programs"
    / "earth"
    / "earth.c"
).read_text(encoding="utf-8")

contracts = (
    "delayInterval(int speed)",
    "if (speed == 0)\n    return 0UL;",
    "main(int argc, char **argv)",
    "NextBitmap(XtPointer client_data, XtIntervalId *id)",
    "input_callback(Widget widget, XtPointer tag, XtPointer callback_data)",
    "speed_callback(Widget widget, XtPointer tag, XtPointer callback_data)",
    "if (AppData.speed != 0)\n        AppData.timeoutID = XtAppAddTimeOut(",
    "AppData.timeoutID = XtAppAddTimeOut(app_con, delayInterval(AppData.speed),",
)
for contract in contracts:
    assert contract in source, f"missing Earth timer/C23 contract: {contract}"

legacy_definitions = (
    "int delayInterval (speed)\n",
    "int main(argc, argv) int argc;",
    "static void NextBitmap (client_data, id)\n",
    "static void speed_callback(widget, tag, callback_data)\n",
)
for legacy in legacy_definitions:
    assert legacy not in source, f"legacy Earth definition remains: {legacy!r}"

print("Earth demo uses typed callbacks and safe zero-speed timer handling")
