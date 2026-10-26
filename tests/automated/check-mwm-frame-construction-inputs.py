#!/usr/bin/env python3
"""Check null-client handling in MWM frame construction and display lists."""

from pathlib import Path


source = Path("clients/mwm/WmCDecor.c").read_text(encoding="utf-8")

construct_start = source.index("Boolean ConstructFrame (ClientData *pcd)")
construct_end = source.index("void GenerateFrameDisplayLists", construct_start)
construct_body = source[construct_start:construct_end]
assert "if (pcd == NULL)" in construct_body
assert "return(FALSE);" in construct_body
assert construct_body.index("if (pcd == NULL)") < construct_body.index("pcd->decor")

display_start = source.index("void GenerateFrameDisplayLists (ClientData *pcd)")
display_end = source.index("void AdoptClient", display_start)
display_body = source[display_start:display_end]
assert "if (pcd == NULL)" in display_body
assert "return;" in display_body
assert display_body.index("if (pcd == NULL)") < display_body.index("pcd->decor")

print("PASS MWM frame construction rejects null clients")
