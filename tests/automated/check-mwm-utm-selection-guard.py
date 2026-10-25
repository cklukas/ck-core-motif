#!/usr/bin/env python3
"""Check that WM selection setup tolerates a missing UTM widget."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "clients/mwm/WmProtocol.c").read_text()


def main() -> None:
    assert "if (wmGD.Screens[scr].utmShell == NULL)" in SOURCE
    assert 'Warning ("Cannot register WM selection without the UTM widget")' in SOURCE
    assert "continue;" in SOURCE
    assert "XtAddCallback(wmGD.Screens[scr].utmShell" in SOURCE


if __name__ == "__main__":
    main()
