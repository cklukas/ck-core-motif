#!/usr/bin/env python3
"""Check Mrm callback value copies handle allocation failures."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
start = text.index("void \nUrm__CW_SafeCopyValue (")
end = text.index("\n\n\n", start)
region = text[start:end]

for allocation, guard in (
    ("int_dst = (int *) XtMalloc ((unsigned int)vec_size)", "if (int_dst == NULL)"),
    ("cstr_table_dst =", "if (cstr_table_dst == NULL)"),
    ("char8_table_dst = (String *) XtMalloc (vec_size)",
     "if (char8_table_dst == NULL && vec_size != 0)"),
    ("int_dst = (int *) XtMalloc (sizeof(int))", "if (int_dst == NULL)"),
    ("char8_dst = (String) XtMalloc (strlen(char8_src)+1)", "if (char8_dst == NULL)"),
    ("single_float_dst = (float *) XtMalloc (sizeof(float))", "if (single_float_dst == NULL)"),
    ("float_dst = (double *) XtMalloc (sizeof(double))", "if (float_dst == NULL)"),
    ("cstr_src = XmStringCopy", "if (cstr_src == NULL)"),
    ("wchar_dst = (wchar_t *) XtMalloc (size)", "if (wchar_dst == NULL)"),
):
    allocation_pos = region.index(allocation)
    guard_pos = region.index(guard, allocation_pos)
    if guard_pos < allocation_pos:
        raise SystemExit(f"guard precedes allocation: {allocation}")
    next_use = region.find("\n", guard_pos)
    if next_use < 0 or "return;" not in region[guard_pos : guard_pos + 100]:
        raise SystemExit(f"allocation guard does not return: {allocation}")

if "(size_t)cnt > (SIZE_MAX / sizeof(wchar_t)) - 1" not in region:
    raise SystemExit("wide-character copy size is not overflow checked")

print("Mrm safe value copies check allocation failures")
