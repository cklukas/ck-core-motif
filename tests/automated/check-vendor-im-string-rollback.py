#!/usr/bin/env python3
"""Check vendor input-method strings are replaced transactionally."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "VendorS.c").read_text()
start = source.index("VendorExtSetValues(\n\t Widget old")
end = source.index("/************************************************************************\n *\n *  SetValues", start)
region = source[start:end]

assert "char                    *replacement;" in region
assert region.count("replacement = nve->input_method_string != NULL") == 1
assert region.count("replacement = nve->preedit_type_string != NULL") == 1
assert region.count("if (nve->input_method_string == NULL || replacement != NULL)") == 1
assert region.count("if (nve->preedit_type_string == NULL || replacement != NULL)") == 1
assert region.count("nve->input_method_string = ove->input_method_string;") == 1
assert region.count("nve->preedit_type_string = ove->preedit_type_string;") == 1
assert region.index("XtNewString(nve->input_method_string)") < region.index("XtFree(ove->input_method_string)")
assert region.index("XtNewString(nve->preedit_type_string)") < region.index("XtFree(ove->preedit_type_string)")
print("vendor input-method strings preserve old values on copy failure")
