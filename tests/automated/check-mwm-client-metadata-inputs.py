#!/usr/bin/env python3
"""Check that MWM client-metadata refreshes reject null client records."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmWinInfo.c").read_text(
    encoding="utf-8"
)


def function_body(name: str) -> str:
    start = source.index(f"\n{name} (")
    end_marker = (
        "\n} /* END OF ProcessMwmHints */"
        if name == "ProcessMwmHints"
        else "\n} /* END OF FUNCTION"
    )
    return source[start : source.index(end_marker, start)]


for function in (
    "ProcessWmClass",
    "ProcessSmClientID",
    "ProcessWmSaveHint",
    "ProcessWmHints",
    "ProcessWmNormalHints",
    "ProcessWmWindowTitle",
    "ProcessWmIconTitle",
    "ProcessMwmHints",
):
    body = function_body(function)
    guard = body.index("if (pCD == NULL)")
    first_member_access = body.index("pCD->")
    assert guard < first_member_access, function
    assert "return;" in body[guard:first_member_access], function

print("PASS MWM client metadata refreshes reject null client records")
