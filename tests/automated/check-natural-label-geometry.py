from pathlib import Path


for name in ("Link.c", "Prompt.c"):
    source = (Path(__file__).parents[2] / "lib" / "Xm" / name).read_text(
        encoding="utf-8"
    )
    assert "request->core.width" in source
    assert "request->core.height" in source
    assert "XmStringExtent" in source
print("PASS Link and Prompt derive natural geometry from requested dimensions")
