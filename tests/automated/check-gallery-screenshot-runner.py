#!/usr/bin/env python3
"""Check the display-backed gallery screenshot runner's safety contract."""
from pathlib import Path
import importlib.util
import subprocess
from unittest.mock import Mock, patch


source = (Path(__file__).parents[2] /
          "tests" / "automated" / "run-gallery-screenshots.py").read_text()

assert 'WINDOW_NAME = "ckmotif-gallery"' in source
assert "xwininfo" in source and '"-root", "-tree"' in source
assert "width > 1 and height > 1" in source
assert '"-id", window_id' in source
assert '[xwd, "-id", window_id, "-silent"]' in source
assert '"-screen"' not in source  # -screen captures the root pixmap, not this window.
assert "xwdtopnm" in source and "pnmtopng" in source
assert "MIN_CAPTURE_PNG_BYTES = 4096" in source
assert "validate_capture_png" in source
assert "capture PNG is implausibly small" in source
assert "capture PNG duplicates an earlier component" in source
assert "process_failure" in source and "process.communicate" in source
assert '"result": "fail"' in source
assert 'receipt["result"] = "skip"' in source
assert 'receipt["result"] = "pass"' in source
assert 'receipt["current_entry"]' in source
assert 'parser.add_argument("--appearance"' in source
assert 'parser.add_argument("--scale"' in source
assert '"--appearance", args.appearance' in source
assert '"--scale", str(args.scale)' in source
assert '"_CKMOTIF_GALLERY_COMPONENT"' in source

# Execute the window-identity polling contract, including transient timeouts.
runner = Path(__file__).with_name("run-gallery-screenshots.py")
spec = importlib.util.spec_from_file_location("gallery_capture", runner)
module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(module)
process = Mock(pid=123)
process.poll.return_value = None
tree = subprocess.CompletedProcess([], 0,
    '  0x100 "ckmotif-gallery": () 900x700+0+0 +0+0\n', "")
identity = subprocess.CompletedProcess(
    [], 0,
    '_NET_WM_PID(CARDINAL) = 123\n_CKMOTIF_GALLERY_READY(CARDINAL) = 1\n'
    '_CKMOTIF_GALLERY_COMPONENT(STRING) = "sample"\n',
    "",
)
wrong_identity = subprocess.CompletedProcess([], 0, "_NET_WM_PID(CARDINAL) = 999\n", "")
for observations in [
    [subprocess.TimeoutExpired("xwininfo", 1), tree, identity],
    [tree, subprocess.TimeoutExpired("xprop", 1), tree, identity],
    [tree, wrong_identity, tree, identity],
]:
    with patch.object(module.subprocess, "run", side_effect=observations) as run:
        assert module.find_window_id("xwininfo", {}, process=process) == "0x100"
        assert run.call_count == len(observations)
    assert process.poll.call_count > 0
process.terminate.assert_not_called()
process.kill.assert_not_called()
ready = subprocess.CompletedProcess(
    [], 0,
    '_NET_WM_PID(CARDINAL) = 123\n_CKMOTIF_GALLERY_READY(CARDINAL) = 1\n'
    '_CKMOTIF_GALLERY_COMPONENT(STRING) = "sample"\n',
    "",
)
viewable = subprocess.CompletedProcess([], 0, "Map State: IsViewable\n", "")
with patch.object(module.subprocess, "run", side_effect=[ready, viewable]):
    module.wait_ready("0x100", process, {}, "sample", timeout=1)
mismatched = subprocess.CompletedProcess(
    [], 0,
    '_NET_WM_PID(CARDINAL) = 123\n_CKMOTIF_GALLERY_READY(CARDINAL) = 1\n'
    '_CKMOTIF_GALLERY_COMPONENT(STRING) = "welcome"\n',
    "",
)
with patch.object(module.subprocess, "run", return_value=mismatched):
    try:
        module.wait_ready("0x100", process, {}, "sample", timeout=0.01)
    except RuntimeError as error:
        assert "did not signal capture readiness" in str(error)
    else:
        raise AssertionError("mismatched component must not be captured")
exited = Mock()
exited.poll.return_value = 1
exited.communicate.return_value = (None, None)
assert "exited with status 1" in str(module.process_failure(exited, "sample"))
with patch.object(module.subprocess, "run") as run:
    try:
        module.find_window_id("xwininfo", {}, timeout=0, process=process)
    except RuntimeError:
        pass
    else:
        raise AssertionError("expired deadline must fail")
    run.assert_not_called()
print("PASS gallery screenshot runner contract")
