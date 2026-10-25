#!/usr/bin/env python3
from pathlib import Path
text=(Path(__file__).parents[2]/"lib"/"Xm"/"List.c").read_text()
start=text.index("XmListPosToBounds(")
end=text.index("/************************************************************************", start+20)
assert "if (w == NULL)" in text[start:end]
assert "if (x      != NULL)" in text[start:end]
print("XmList position bounds validate null widgets")
