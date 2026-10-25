# Deterministic display-scale override

`<Xm/Display.h>` now exposes an additive test/developer hook for simulating a
display scale without changing the caller's X resource database:

```c
XmSetDisplayScaleOverride(display, 2.0);
/* create or update widgets at a deterministic 2x default scale */
XmClearDisplayScaleOverride(display);
```

`XmGetDisplayScaleOverride` reports whether an override is active.  The setter
accepts only finite, positive values; invalid values leave the current value
untouched.  Overrides are stored per `Display`, are process-local, and are
removed when cleared or when the Motif display object is destroyed.  They do
not call `XrmPutStringResource`, alter `Xft.dpi`, or rewrite an application's
resource database.

The override is consulted before the optional XLibre output-DPI adapter and
the existing global `Xft.dpi` fallback.  A changed value is delivered through
the same UI-thread transaction boundary used by the XLibre observer.  The
transaction first lets registered widgets update toolkit-owned derived
defaults, then invokes the normal same-size resize path.  Widget identity,
callbacks, focus, selections, and application-owned pixel resources are not
replaced by the transaction.  Only toolkit-owned defaults that already
participate in the DPI contract are affected.

This increment provides live relayout for the registered scale-aware subset;
it does not claim that every historical Motif metric reflows or that a real
multi-output server has been qualified.  Those remain separate work-package
gates with stronger geometry and XLibre evidence requirements.

The installed consumer fixture is
`tests/automated/run-display-scale-override.py`.  It checks invalid values,
per-display retention, resource-database identity, default indicator scaling,
explicit pixel-resource preservation, and reversible cleanup against the
selected installed `libXm`.

## Qualification evidence

The first qualification ran on the AArch64 `uconsole` Linux host from a
fresh Clang/Xft/Unicode/AT-SPI installed prefix.  The current installed
`libXm.so.8.0.0` fingerprint is
`db1cb5845391519431410918d11dbc2e64daf8e4a18c13318151b20c52319d99`; the
fixture source fingerprint is
`30f791c20513b62d4fc1d6000e036cb310cdb4ad3b9ff0d07eaa5e50da72dda5`.
Three independent Xvfb runs passed in
`/tmp/ckmotif-scale-20261025/results/scale-override-r6/`, `-r7/` and `-r8/`.
A consumer-only ASan/UBSan run with leak detection enabled also passed; its
output is retained in
`/tmp/ckmotif-scale-20261025/results/scale-override-asan-r2/`.
The expanded precedence fixture was rerun in `scale-override-r9/`, `-r10/`
and `-r11/`, with a separate strict run in `scale-override-asan-r3/`.
The live-transaction fixture was then rebuilt against the updated library and
passed in `fixture-r5/`, `-r6/` and `-r7/`, with a consumer-only strict
ASan/UBSan run in `asan-r4/`; the installed library fingerprint for that run
is `bdd7689d479646e4f7d03f297626c713194bd7128b2e27961959104922ef30a3`.
The SpinBox live-default extension was subsequently rebuilt and passed in
`fixture-spin-r3-4/`, `-5/` and `-6/`, with a consumer-only strict ASan/UBSan
run in `fixture-spin-asan-r3/`. That installed `libXm.so.8.0.0` is
`3dc9b995cb71894e66c40d013ec0bac242f6e9633d0d60e343ac4c2ff8b74b45`, and the
fixture source is `2001413c96751c6bab59f6794ed54c0b9ea66a256880ef1b0b8d36e029c8fb66`.
The same installed prefix then qualified ProgressRing preferred-size reflow in
`fixture-ring-r1-2/`, `-3/` and `-4/`, with a consumer-only strict ASan/UBSan
run in `fixture-ring-asan-r1/`. The fixture source for that combined cohort is
`5cb2a55003cab57222f4f06cf1a934a68492c3c8338a526c56db1ee9c3dfd106`.
The C23 configure probe was separately run with GCC 14 and reported the C11
atomic capability as available.  The expanded fixture evidence qualifies the
same-thread transaction, default indicator reflow, explicit-size preservation,
toggle-state preservation, SpinBox and ProgressRing default reflow, and
reversible cleanup;
full historical-widget coverage and live appearance transactions remain open.
