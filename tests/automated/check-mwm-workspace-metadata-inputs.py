#!/usr/bin/env python3
"""Check null-input handling for WSM workspace metadata helpers."""

from pathlib import Path


source = Path("clients/mwm/WmProperty.c").read_text(encoding="utf-8")


def body(marker: str, end_marker: str) -> str:
    start = source.index(marker)
    end = source.index(end_marker, start)
    return source[start:end]


for marker, end_marker in (
    ("SetWorkspaceInfoProperty (WmWorkspaceData *pWS)",
     "} /* END OF FUNCTION SetWorkspaceInfoProperty */"),
    ("DeleteWorkspaceInfoProperty (WmWorkspaceData *pWS)",
     "} /* END OF FUNCTION DeleteWorkspaceInfoProperty */"),
):
    section = body(marker, end_marker)
    assert "pWS == NULL" in section
    assert "pWS->pSD == NULL" in section
    assert "return;" in section

name_helper = body("WorkspacePropertyName (WmWorkspaceData *pWS)",
                   "} /* END OF FUNCTION WorkspacePropertyName */")
assert "pWS == NULL" in name_helper
assert "pWS->name == NULL" in name_helper
assert "return (NULL);" in name_helper

print("PASS MWM workspace metadata helpers reject null inputs")
