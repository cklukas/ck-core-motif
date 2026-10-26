#!/usr/bin/env python3
"""Check idempotent localized message initialization in MWM."""

from pathlib import Path


source = Path("clients/mwm/WmFeedback.c").read_text(encoding="utf-8")
start = source.index("initMesg()")
end = source.index("static char *confirm_widget", start)
body = source[start:end]

assert "static Boolean messagesInitialized = False;" in body
assert "if (messagesInitialized)" in body
assert "messagesInitialized = True;" in body
assert body.index("if (messagesInitialized)") < body.index("GETMESSAGE")

print("PASS MWM message initialization is idempotent")
