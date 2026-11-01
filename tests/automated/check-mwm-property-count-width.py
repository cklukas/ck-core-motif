#!/usr/bin/env python3
"""Check MWM property writers reject counts too large for Xlib."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmProperty.c").read_text(
    encoding="utf-8"
)


def body(marker: str, end_marker: str) -> str:
    start = source.index(marker)
    return source[start:source.index(end_marker, start)]


for marker, end_marker, parameter in (
    ("void SetEmbeddedClientsProperty", "} /* END OF FUNCTION SetEmbeddedClientsProperty", "cEmbeddedClients"),
    ("void SetWorkspacePresence", "} /* END OF FUNCTION SetWorkspacePresence", "cPresence"),
    ("void SetMwmClientList", "} /* END OF FUNCTION SetMwmClientList", "wmWindowCount"),
):
    section = body(marker, end_marker)
    assert f"{parameter} >" in section and "INT_MAX" in section
    assert f"(int){parameter}" in section

workspace_info = body("void SetWorkspaceInfo", "} /* END OF FUNCTION SetWorkspaceInfo")
assert "size_t element_count;" in workspace_info
assert "element_count > (size_t)INT_MAX" in workspace_info
assert "(int)element_count" in workspace_info

workspace_list = body("SetWorkspaceListProperty (WmScreenData *pSD)",
                      "} /* END OF FUNCTION SetWorkspaceListProperty")
assert "size_t element_count;" in workspace_list
assert "element_count > (size_t)INT_MAX" in workspace_list
assert "(int)element_count" in workspace_list

print("MWM property writers bound Xlib element counts")
