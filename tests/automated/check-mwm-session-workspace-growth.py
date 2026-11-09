#!/usr/bin/env python3
"""Check that MWM session workspace serialization fits Xt's width."""

from pathlib import Path


SOURCE = Path(__file__).parents[2] / "clients" / "mwm" / "WmXSMP.c"


def main() -> None:
    text = SOURCE.read_text(encoding="utf-8")
    start = text.index("getClientWorkspaces(ClientData *pCD)\n{")
    end = text.index("\n}\n\n#endif /* WSM */", start)
    region = text[start:end]
    assert "pCD == NULL || pSD == NULL || pCD->numInhabited <= 0 ||" in region
    assert "pCD->pWsList == NULL" in region
    assert region.index("pCD->pWsList == NULL") < region.index(
        "for (i = 0; i < pCD->numInhabited; i++)"
    )
    assert "nameLength >= UINT_MAX" in region
    assert "pLen > UINT_MAX" in region
    assert "XtMalloc(pLen)" in region
    assert "XtRealloc(cwsP, pLen)" in region
    print("MWM session workspace serialization is width-checked")


if __name__ == "__main__":
    main()
