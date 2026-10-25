#!/usr/bin/env python3
"""Check that MWM icon-box hierarchy construction is transactional."""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SOURCE = (ROOT / "clients/mwm/WmIconBox.c").read_text()
HEADER = (ROOT / "clients/mwm/WmIconBox.h").read_text()


def main() -> None:
    assert "extern Boolean MakeShell" in HEADER
    assert "extern Boolean MakeScrolledWindow" in HEADER
    assert "extern Boolean MakeBulletinBoard" in HEADER
    assert "extern Boolean RealizeIconBox" in HEADER
    assert "if (!MakeShell (pWS, pIBD) ||" in SOURCE
    assert "if (pIBD->shellWidget == NULL)" in SOURCE
    assert "if (pIBD->frameWidget == NULL)" in SOURCE
    assert "if (pIBD->scrolledWidget == NULL)" in SOURCE
    assert "return (pIBD->bBoardWidget != NULL);" in SOURCE
    assert "if (pIBD->shellWidget == NULL || pIBD->scrolledWidget == NULL ||" in SOURCE
    assert "if (!XtIsRealized (pIBD->shellWidget))" in SOURCE
    assert "if (clipWidget == NULL)" in SOURCE
    assert "XtFree ((char *) pIBD->IPD.placeList);" in SOURCE


if __name__ == "__main__":
    main()
