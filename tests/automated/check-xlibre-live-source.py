#!/usr/bin/env python3
"""Check the live XLibre fixture's safety and qualification boundaries."""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
fixture = (ROOT / "tests/automated/check-xlibre-live.c").read_text()
runner = (ROOT / "tests/automated/run-xlibre-live.py").read_text()

# The fixture must be a real property mutation test, not a parser-only probe.
assert "XRRChangeOutputProperty" in fixture
assert fixture.count("XRRChangeOutputProperty") == 2
assert "XSync(display, False);" in fixture
assert "wait_for_callback(app) || callback_count != 1" in fixture
assert "callback_old_output != output || callback_new_output != output" in fixture
assert "callback_old_scale - old_dpi / _XM_XLIBRE_DPI_BASE" in fixture
assert "callback_new_scale - new_dpi / _XM_XLIBRE_DPI_BASE" in fixture

# Every successful mutation must restore the original property before the
# observer and shell are retired.  Keep these checks relative to cleanup so a
# future edit cannot accidentally move restoration after teardown.
cleanup = fixture[fixture.index("cleanup:") :]
assert "if (property_changed)" in cleanup
assert "property_value = old_dpi;" in cleanup
restore = cleanup.index("property_value = old_dpi;")
uninstall = cleanup.index("_XmXlibreDpiUninstallShell(shell);")
destroy = cleanup.index("XtDestroyWidget(shell);")
assert restore < uninstall < destroy
assert "Boolean property_changed = False;" in fixture
assert "Boolean observer_installed = False;" in fixture
assert "if (observer_installed)" in cleanup

# A generic Xvfb run must be an explicit non-qualification result, while a
# caller-provided display must require the fixture's real pass marker.
assert "return 77;" in fixture
assert '"--display"' in runner
assert "result.returncode == 77" in runner
assert '"PASS XLibre live output-property transaction"' in runner
assert '"SKIP XLibre live observer"' in runner
assert '"source_hashes"' in runner
assert '"installed_lib_sha256"' in runner
assert '"receipt.json"' in runner
assert '"ordinary-xvfb-fallback"' in runner
assert '"result": "pending"' in runner
assert 'receipt["result"] = "pass"' in runner
assert 'else "skip"' in runner
assert 'receipt["result"] = "fail"' in runner

print("PASS XLibre live fixture source safety and qualification contract")
