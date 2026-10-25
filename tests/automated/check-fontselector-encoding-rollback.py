#!/usr/bin/env python3
"""Check FontSelector encoding replacement is transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FontS.c").read_text()
change_start = source.index("ChangeEncoding(Widget w")
change_end = source.index("/*\tFunction Name: ToggleScaling", change_start)
change = source[change_start:change_end]
set_start = source.index("static Boolean \nSetValues")
set_end = source.index("/*\tFunction Name: GetValuesHook", set_start)
set_region = source[set_start:set_end]

assert "String replacement;" in change
assert "if (replacement != NULL)" in change
assert change.index("XtNewString(ANY_ENCODING)") < change.index("XtFree(ENCODING_STRING(fsw))")
assert "String encoding_copy;" in set_region
assert "encoding_copy = ENCODING_STRING(set_fsw) != NULL" in set_region
assert "ENCODING_STRING(set_fsw) = ENCODING_STRING(old_fsw);" in set_region
assert set_region.index("XtNewString(ENCODING_STRING(set_fsw))") < set_region.index("XtFree(ENCODING_STRING(old_fsw))")
print("FontSelector encoding preserves old values on copy failure")
