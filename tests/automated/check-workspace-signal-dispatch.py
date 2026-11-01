#!/usr/bin/env python3
"""Normal workspace-manager signals must defer toolkit work to the Xt loop."""
from pathlib import Path
root = Path(__file__).resolve().parents[2]
for path, signature, notice in (
    ('clients/mwm/WmSignal.c', 'void QuitWmSignalHandler (int dummy)', 'quitSignal'),
    ('demos/programs/workspace/wsm.c', 'static void QuitWsmSignalHandler (int dummy)', 'quit_signal'),
):
    source = (root / path).read_text()
    body = source.split(signature, 1)[1].split('}', 1)[0]
    assert 'XtNoticeSignal(' + notice + ')' in body
    assert 'saved_errno = errno' in body and 'errno = saved_errno' in body
    assert 'Do_Quit' not in body and 'ManageAllWindows' not in body and 'ConfirmAction' not in body
    assert 'XFlush' not in body
    assert 'XtAppAddSignal(' in source
init = (root / 'clients/mwm/WmInitWs.c').read_text()
assert init.index('XtCreateApplicationContext()') < init.index('SetupWmSignalHandlers (0)')
print('PASS normal termination handlers only notify the Xt event loop')
