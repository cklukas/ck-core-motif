#!/usr/bin/env python3
"""Withdrawn client contexts must be removed before destructors clear their IDs."""
from pathlib import Path
source = (Path(__file__).resolve().parents[2] / 'clients/mwm/WmManage.c').read_text()
body = source.split('void WithdrawWindow (ClientData *pCD)', 1)[1].split('END OF FUNCTION WithdrawWindow', 1)[0]
assert body.count('DeleteClientContext(pCD);') == 1
assert body.index('DeleteClientContext(pCD);') < body.index('FreeClientFrame (pCD)')
assert body.index('DeleteClientContext(pCD);') < body.index('FreeIcon (pCD)')
print('PASS client event associations detach before frame/icon IDs are cleared')
