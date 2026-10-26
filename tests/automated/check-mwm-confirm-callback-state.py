#!/usr/bin/env python3
"""Check callback-data and action bounds in the MWM confirmation callback."""

from pathlib import Path


source = Path("clients/mwm/WmFeedback.c").read_text(encoding="utf-8")
start = source.index("static void OkCB")
end = source.index("END OF FUNCTION OkCB", start)
body = source[start:end]

assert "client_data == NULL" in body
assert "actionNbr < 0" in body
assert "actionNbr >= 4" in body
assert body.index("client_data == NULL") < body.index("confirm_func[")
assert body.index("actionNbr < 0") < body.index("confirm_func[")

print("PASS MWM confirmation callback state is bounded")
