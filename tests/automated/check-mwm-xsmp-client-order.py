from pathlib import Path

source = Path("clients/mwm/WmXSMP.c").read_text(encoding="utf-8")
start = source.index("static int\nclientWorkspaceCompare")
end = source.index("/*\n *  Assumes: wmGD.clientResourceDB", start)
region = source[start:end]

assert "pCD1->pSD->screen < pCD2->pSD->screen" in region
assert "pCD1->pSD->screen - pCD2->pSD->screen" not in region
assert "wsID <" in region and "wsID >" in region
assert "(int)(pCD1->pWsList" not in region
print("PASS MWM XSMP client ordering avoids overflowing subtraction")
