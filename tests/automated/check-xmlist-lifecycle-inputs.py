#!/usr/bin/env python3
"""Check internal XmList helpers reject null lifecycle inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "xmlist.c").read_text()
for function, guard in (
    ("_XmQueuePop(", "if (queue == NULL)"),
    ("_XmQueueCount(", "if (queue == NULL)"),
    ("_XmListAddAfter(", "if (list == NULL)"),
    ("_XmListAddBefore(", "if (list == NULL)"),
    ("_XmListRemove(", "if (list == NULL || elem == NULL)"),
    ("_XmListExec(", "if (list == NULL || func == NULL)"),
):
    start = source.index("\n" + function) + 1
    end = source.index("\n}\n", start) + 3
    assert guard in source[start:end], function
print("PASS internal XmList lifecycle input guards")
