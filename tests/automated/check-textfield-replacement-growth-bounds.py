#!/usr/bin/env python3
"""Check TextField replacement growth rejects unrepresentable capacities."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c"
text = source.read_text(encoding="utf-8")
start = text.index("_XmTextFieldReplaceText(")
end = text.index("\n}\n", start)
first = text[start:end]
start = text.index("_XmTextFieldReplaceTextForPreedit(")
end = text.index("\n}\n", start)
second = text[start:end]
region = first + second
helper_start = text.index("TextFieldCheckedGrowth")
helper_end = text.index("/********    End Static Function Declarations", helper_start)
helper = text[helper_start:helper_end]

if "result_length > (size_t)INT_MAX" not in helper:
    raise SystemExit("growth helper lacks signed length bound")
if "target > UINT_MAX" not in helper:
    raise SystemExit("growth helper lacks capacity bound")
if region.count("TextFieldCheckedGrowth(") != 4:
    raise SystemExit("replacement paths do not use the checked growth helper")

print("TextField replacement growth rejects unrepresentable capacities")
