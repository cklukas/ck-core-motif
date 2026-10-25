#!/usr/bin/env python3
"""Check that WSM requests without a registered callback fail safely."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWsmLib/recv.c"
text = source.read_text(encoding="utf-8")

needle = "if (screen_info == NULL || screen_info->request_callback == NULL)"
assert needle in text
guard = text[text.index(needle):text.index(needle) + 180]
assert "FreeRequest(&request);" in guard
assert "return(False);" in guard
print("WSM requests without callbacks are rejected safely")
