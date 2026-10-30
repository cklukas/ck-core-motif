#!/usr/bin/env python3
"""Check TeachingTip text updates preserve ownership on copy failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/TeachingTip.c").read_text()
start = source.rindex("static Boolean SetValues")
end = source.index("static XtGeometryResult QueryGeometry", start)
body = source[start:end]

assert body.count("XmString replacement = copy_string") == 2
assert body.count("if (replacement)") == 2
assert body.count("tip->teaching_tip.title = old->teaching_tip.title") == 1
assert body.count("tip->teaching_tip.text = old->teaching_tip.text") == 1
assert "return changed;" in body

destroy = source[source.index("static void Destroy") : start]
assert "XmStringFree(tip->teaching_tip.title)" in destroy
assert "XmStringFree(tip->teaching_tip.text)" in destroy

print("TeachingTip SetValues preserves text resources and reports changes")
