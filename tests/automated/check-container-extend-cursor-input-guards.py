#!/usr/bin/env python3
"""Check Container cursor extension validates action inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.index("ContainerExtendCursor(\n        Widget")
end = source.index("ContainerToggleMode(\n        Widget", start)
region = source[start:end]
guard = "if (event == NULL || num_params == NULL || *num_params == 0 ||"
assert guard in region
assert "params == NULL)" in region
assert region.index(guard) < region.index("CalcNextLocationCursor")

print("PASS Container cursor extension validates action inputs")
