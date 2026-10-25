#!/usr/bin/env python3
"""Check that gallery capture options are applied before widget creation."""
from pathlib import Path

source = (Path(__file__).parents[2] / "demos/programs/ckmotif-gallery/gallery.c").read_text()
shell = source.index('shell = XtVaAppInitialize')
profile = source.index('XmSetAppearanceProfile', shell)
children = source.index('root = XmVaCreateManagedForm', shell)
realize = source.index('XtRealizeWidget(shell)', shell)
assert profile < children < realize
assert 'XmApplyAppearanceProfile(XtDisplay(shell), requested_profile_value)' not in source
assert 'XmSetDisplayScaleOverride(XtDisplay(shell), requested_scale_value)' in source
print('PASS gallery capture options are selected before child creation')
