#!/usr/bin/env python3
"""Check the Text source scan callback validates source state first."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c").read_text(
    encoding="utf-8"
)
start = source.index("static XmTextPosition \nScan(")
end = source.index("\n  switch (sType)", start)
region = source[start:end]

assert "source == NULL || source->data == NULL" in region
assert "source->data->widgets == NULL" in region
assert "source->data->numwidgets <= 0" in region
assert "source->data->widgets[0] == NULL" in region
assert "return 0;" in region

print("Text source scan rejects missing source state")
