from pathlib import Path

source = Path("clients/mwm/WmWinInfo.c").read_text(encoding="utf-8")
start = source.index("if (pCD->pSD == NULL")
body = source[start:source.index("pCD->sizeWsList", start)]

assert body.count("XtFree ((char *)pCD)") == 2
assert body.count("return (NULL);") == 2
print("PASS MWM workspace-list failures release client data")
