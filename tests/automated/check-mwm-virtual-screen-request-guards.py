#!/usr/bin/env python3
"""Unimplemented virtual-root requests must not be advertised or decoded."""
from pathlib import Path
source = (Path(__file__).parents[2] / 'clients/mwm/WmProtocol.c').read_text()
region = source[source.index('static Boolean\nWMiConvert ('):]
assert 'PanRoot' not in region
assert '_WSMReqTypeToTarget(DISPLAY, WSM_WM_PAN)' not in region
assert '_WSMReqTypeToTarget(DISPLAY, WSM_WM_GOTO)' not in region
assert '_WSMTargetToReqType(DISPLAY, target) <= WSM_REG_WINDOW' in region
assert 'WSMProcessProtoTarget' in region
print('PASS only implemented workspace targets are advertised and dispatched')
