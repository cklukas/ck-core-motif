from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/DropDown.c").read_text(
    encoding="utf-8"
)
start = source.index("static void\nGetTextAndLabelWidth")
body = source[start:source.index("static void\n", start + 20)]

assert "long long text_and_label" in body
assert "Keep the human-readable label intact" in body
assert "text_width * text_and_label" not in body
assert "minimum_label" in body
print("PASS DropDown preserves label width while shrinking editable text")
