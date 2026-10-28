#!/usr/bin/env python3
"""Check TextField replacement growth uses one checked capacity helper."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()

helper = source[source.index("TextFieldCheckedGrowth") : source.index("/********    End Static Function Declarations", source.index("TextFieldCheckedGrowth"))]
assert "string_length < 0" in helper
assert "current + inserted < replaced" in helper
assert "result_length > (size_t)INT_MAX" in helper
assert "SIZE_MAX / unit_size" in helper
assert "target > UINT_MAX" in helper

replace_start = source.index("_XmTextFieldReplaceText(")
preedit_start = source.index("_XmTextFieldReplaceTextForPreedit")
replace_body = source[replace_start:preedit_start]
preedit_body = source[preedit_start:]
assert replace_body.count("TextFieldCheckedGrowth(") == 2
assert preedit_body.count("TextFieldCheckedGrowth(") == 2
assert "tf->text.string_length + insert_length - replace_length >=" not in source
assert "(insert_length + TEXT_INCREMENT) * sizeof(wchar_t)" not in source
print("TextField replacement growth arithmetic is checked")
