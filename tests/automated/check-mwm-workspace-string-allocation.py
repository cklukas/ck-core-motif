#!/usr/bin/env python3
"""Check that MWM workspace strings use the checked catalog copier."""

from pathlib import Path


SOURCE = Path(__file__).parents[2] / "clients" / "mwm" / "WmResource.c"


def main() -> None:
    text = SOURCE.read_text(encoding="utf-8")
    workspace_start = text.index("ProcessWorkspaceList (WmScreenData")
    workspace_end = text.index("} /* END OF FUNCTION ProcessWorkspaceList", workspace_start)
    workspace = text[workspace_start:workspace_end]
    assert "pwsI->name = CopyCatalogString((char *)string)" in workspace
    assert "XtMalloc (1+strlen((char *)string))" not in workspace

    presence_start = text.index("ProcessPresenceResources (WmScreenData")
    presence_end = text.index("} /* END OF FUNCTION ProcessPresenceResources", presence_start)
    presence = text[presence_start:presence_end]
    assert "CopyCatalogString((char *)pch1)" in presence
    assert "XtMalloc (1+strlen((char *)pch1))" not in presence
    print("MWM workspace strings use width-checked copies")


if __name__ == "__main__":
    main()
