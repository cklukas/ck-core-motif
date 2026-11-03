#!/usr/bin/env python3
"""Check the non-MINIMAL_DT quit-message fallback has stable lifetime."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmFeedback.c").read_text()
marker = 'if ((confirm_mesg[3] ='
start = source.index(marker)
end = source.index('\n\n    messagesInitialized = True;', start)
region = source[start:end]
assert 'confirm_mesg[3] = "QUIT Workspace Manager?";' in region
assert 'confirm_mesg[3] = ((char *)GETMESSAGE' not in region
assert '\n\ttmpString = ((char *)GETMESSAGE(22, 11, "QUIT Workspace Manager?"));\n#endif /* MINIMAL_DT */' not in region
print("mwm feedback quit-message fallback has stable storage")
