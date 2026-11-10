#!/usr/bin/env python3
from pathlib import Path
source=(Path(__file__).parents[2]/"lib/Xm/Stepper.c").read_text()
s="".join(source.split())
assert "!s||!XmIsStepper((Widget)s)||s->core.being_destroyed||!event||event->type!=KeyPress||!XtIsSensitive((Widget)s)||!XtIsSensitive(child)" in s
print("PASS stepper-event-guards")
