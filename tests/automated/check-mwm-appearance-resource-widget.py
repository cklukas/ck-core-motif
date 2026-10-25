#!/usr/bin/env python3
"""Check that MWM appearance resource-widget creation is guarded."""

from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "clients/mwm/WmResource.c").read_text()


def main() -> None:
    assert "clientW = XtCreateWidget (WmNclient" in SOURCE
    assert "if (clientW == NULL)" in SOURCE
    assert 'Warning ("Cannot create the client appearance resource widget")' in SOURCE
    assert "ExitWM (WM_ERROR_EXIT_VALUE);" in SOURCE


if __name__ == "__main__":
    main()
