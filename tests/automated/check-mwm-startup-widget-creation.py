#!/usr/bin/env python3
"""Check that MWM startup widget creation handles allocation failures."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "clients/mwm/WmInitWs.c").read_text()


def main() -> None:
    assert "if (pSD->screenTopLevelW == NULL)" in SOURCE
    assert 'Warning ("Cannot create the screen resource shell")' in SOURCE
    assert "if (pSD->utmShell != NULL)" in SOURCE
    assert "if (pSD->screenTopLevelW1 == NULL)" in SOURCE
    assert "if (pWS->workspaceTopLevelW == NULL)" in SOURCE
    assert 'Warning ("Cannot create the workspace resource widget")' in SOURCE


if __name__ == "__main__":
    main()
