#!/usr/bin/env python3
"""Check Container enter/leave handlers validate event inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
enter = source[source.rindex("EnterHandler("):source.index("\n}", source.rindex("EnterHandler("))]
leave = source[source.rindex("LeaveHandler("):source.index("\n}", source.rindex("LeaveHandler("))]
assert enter.index("if (wid == NULL)") < enter.index("cw->container.LeaveDir")
assert leave.index("if (wid == NULL || event == NULL || clip == NULL)") < leave.index("event->xcrossing")
print("PASS Container enter/leave handlers validate widget and event inputs")
