#!/usr/bin/env python3
"""Run WP-48 MWM modern frame appearance, DPI/density scaling, and override tests on Xvfb."""
from __future__ import annotations

import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import platform
import re
import select
import shutil
import subprocess
import sys
import time


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def parse_metrics(output: str) -> dict[str, str | int]:
    for line in output.splitlines():
        if line.startswith("METRICS "):
            metrics: dict[str, str | int] = {}
            for token in line.split()[1:]:
                key, value = token.split("=", 1)
                if value.isdigit() or (value.startswith("-") and value[1:].isdigit()):
                    metrics[key] = int(value)
                else:
                    metrics[key] = value
            return metrics
    raise RuntimeError(f"missing METRICS line in probe output:\n{output}")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--prefix", required=True, type=Path)
    parser.add_argument("--out", required=True, type=Path)
    args = parser.parse_args()

    prefix = args.prefix.resolve(strict=True)
    output = args.out.resolve()
    if output.exists() or output.is_relative_to(prefix) or prefix.is_relative_to(output):
        parser.error("--out must be a fresh directory separate from --prefix")
    if platform.system() == "Darwin" and not any(
        base.parent.is_mount() and output.is_relative_to(base)
        for base in (Path("/Volumes/S8T/tmp"), Path("/Volumes/PRO-BLADE/tmp"))
    ):
        parser.error("Mac results must be beneath a mounted project scratch volume")

    output.mkdir(parents=True)
    (output / "tmp").mkdir()
    (output / "home").mkdir()

    root = Path(__file__).resolve().parents[2]
    source = root / "tests/automated/check-mwm-modern-appearance.c"

    # Support both installed prefix (run-suite.py) and build tree
    ext = ".dylib" if platform.system() == "Darwin" else ".so"
    if (prefix / "bin/mwm").is_file():
        mwm_bin = (prefix / "bin/mwm").resolve(strict=True)
        lib_dir = (prefix / "lib").resolve(strict=True)
        library = (lib_dir / f"libXm{ext}").resolve(strict=True)
        inc_flags = ["-I" + str(prefix / "include")]
    else:
        mwm_candidate = prefix / "clients/mwm/.libs/mwm"
        if not mwm_candidate.is_file():
            mwm_candidate = prefix / "clients/mwm/mwm"
        mwm_bin = mwm_candidate.resolve(strict=True)
        lib_dir = (prefix / "lib/Xm/.libs").resolve(strict=True)
        library = (lib_dir / f"libXm{ext}").resolve(strict=True)
        inc_flags = ["-I" + str(prefix / "lib"), "-I" + str(root / "lib")]

    base_env = dict(
        os.environ,
        TMPDIR=str(output / "tmp"),
        HOME=str(output / "home"),
        XDG_CACHE_HOME=str(output / "tmp"),
        PYTHONDONTWRITEBYTECODE="1",
        LC_ALL="en_US.UTF-8" if platform.system() == "Darwin" else "C.UTF-8",
    )
    for var in ("DISPLAY", "XENVIRONMENT", "XAPPLRESDIR", "LD_PRELOAD", "LD_AUDIT"):
        base_env.pop(var, None)
    lib_var = "DYLD_LIBRARY_PATH" if platform.system() == "Darwin" else "LD_LIBRARY_PATH"
    base_env[lib_var] = str(lib_dir)

    receipt = {
        "started_utc": datetime.datetime.now(datetime.timezone.utc).isoformat(),
        "mwm": str(mwm_bin),
        "mwm_sha256": sha256(mwm_bin),
        "library": str(library),
        "library_sha256": sha256(library),
        "source": str(source),
        "source_sha256": sha256(source),
        "cases": {},
        "success": False,
    }
    started = time.monotonic()

    probe_bin = output / "check-mwm-modern-appearance"
    cc = os.environ.get("CC", "cc")
    cflags = ["-std=c99", "-Wall", "-Wextra", "-Werror", "-O2", *inc_flags]
    if platform.system() == "Darwin":
        cflags += ["-I/opt/X11/include", "-L/opt/X11/lib"]
    compile_cmd = [cc, *cflags, str(source), "-lX11", "-o", str(probe_bin)]
    compile_res = subprocess.run(
        compile_cmd, env=base_env, capture_output=True, text=True, timeout=60
    )
    (output / "compile.log").write_text(compile_res.stdout + compile_res.stderr)
    if compile_res.returncode != 0:
        print(compile_res.stdout + compile_res.stderr, file=sys.stderr)
        return 1

    # Write a minimal .mwmrc to verify configFile loading alongside modern appearance
    mwmrc_path = output / "home/.mwmrc"
    mwmrc_path.write_text(
        'Menu DefaultWindowMenu\n{\n    "Restore" _R f.normalize\n    "Close" _C f.kill\n}\n',
        encoding="utf-8",
    )

    xvfb = shutil.which("Xvfb") or "/opt/X11/bin/Xvfb"
    read_fd, write_fd = os.pipe()
    server = None
    try:
        with (output / "xvfb.log").open("w") as xvfb_log:
            server = subprocess.Popen(
                [
                    xvfb,
                    "-displayfd",
                    str(write_fd),
                    "-screen",
                    "0",
                    "1600x1200x24",
                    "-noreset",
                    "-nolisten",
                    "tcp",
                ],
                env=base_env,
                pass_fds=(write_fd,),
                stdout=xvfb_log,
                stderr=subprocess.STDOUT,
            )
        os.close(write_fd)
        write_fd = -1
        with os.fdopen(read_fd) as channel:
            read_fd = -1
            if not select.select([channel], [], [], 10)[0]:
                raise RuntimeError("Xvfb did not publish its isolated display")
            disp_num = channel.readline().strip()
            if not disp_num.isdecimal():
                raise RuntimeError(f"Xvfb returned invalid display number: {disp_num!r}")
            display = ":" + disp_num

        def run_case(
            name: str,
            xresources: str,
            probe_args: list[str],
            extra_mwm_args: list[str] | None = None,
        ) -> dict[str, str | int]:
            xenv_path = output / f"{name}.Xresources"
            xenv_path.write_text(xresources, encoding="utf-8")
            case_env = dict(base_env, DISPLAY=display, XENVIRONMENT=str(xenv_path))
            mwm_cmd = [
                str(mwm_bin),
                "-xrm",
                "Mwm*keyboardFocusPolicy: explicit",
                "-xrm",
                "Mwm*startupKeyFocus: True",
                "-xrm",
                "Mwm*showFeedback: none",
                "-xrm",
                f"Mwm*configFile: {mwmrc_path}",
                *(extra_mwm_args or []),
            ]
            mwm_log_path = output / f"{name}-mwm.log"
            with mwm_log_path.open("w") as mwm_log:
                mwm_proc = subprocess.Popen(
                    mwm_cmd, env=case_env, stdout=mwm_log, stderr=subprocess.STDOUT
                )
                try:
                    time.sleep(0.35)
                    if mwm_proc.poll() is not None:
                        raise RuntimeError(
                            f"mwm exited early in {name}:\n"
                            + mwm_log_path.read_text(errors="replace")
                        )
                    probe_res = subprocess.run(
                        [str(probe_bin), *probe_args],
                        env=case_env,
                        capture_output=True,
                        text=True,
                        timeout=30,
                    )
                    (output / f"{name}-probe.log").write_text(
                        probe_res.stdout + probe_res.stderr
                    )
                    if probe_res.returncode != 0:
                        raise RuntimeError(
                            f"probe failed in {name}:\n"
                            + probe_res.stdout
                            + probe_res.stderr
                        )
                    metrics = parse_metrics(probe_res.stdout)
                    receipt["cases"][name] = metrics
                    return metrics
                finally:
                    if mwm_proc.poll() is None:
                        mwm_proc.terminate()
                        try:
                            mwm_proc.wait(timeout=5)
                        except subprocess.TimeoutExpired:
                            mwm_proc.kill()
                            mwm_proc.wait()

        # 1. Classic profile at 1x (96 DPI)
        m_classic = run_case(
            "classic-1x",
            "*appearanceProfile: classic\n*appearanceDensity: comfortable\nXft.dpi: 96\n",
            ["--profile", "classic"],
        )

        # 2. Modern-light comfortable at 1x (96 DPI)
        m_light_1x = run_case(
            "modern-light-1x",
            "*appearanceProfile: modern-light\n*appearanceDensity: comfortable\nXft.dpi: 96\n",
            [
                "--profile",
                "modern-light",
                "--expect-active-bg",
                "#DCE6F7",
                "--expect-inactive-bg",
                "#E8ECF2",
                "--expect-active-border",
                "#2563EB",
            ],
        )

        # 3. Modern-light compact at 1x (96 DPI)
        m_light_compact = run_case(
            "modern-light-compact-1x",
            "*appearanceProfile: modern-light\n*appearanceDensity: compact\nXft.dpi: 96\n",
            [
                "--profile",
                "modern-light",
                "--expect-active-bg",
                "#DCE6F7",
                "--expect-inactive-bg",
                "#E8ECF2",
                "--expect-active-border",
                "#2563EB",
            ],
        )

        # 4. Modern-light comfortable at 2x (192 DPI)
        m_light_2x = run_case(
            "modern-light-2x",
            "*appearanceProfile: modern-light\n*appearanceDensity: comfortable\nXft.dpi: 192\n",
            [
                "--profile",
                "modern-light",
                "--expect-active-bg",
                "#DCE6F7",
                "--expect-inactive-bg",
                "#E8ECF2",
                "--expect-active-border",
                "#2563EB",
            ],
        )

        # 5. Modern-dark at 1x (96 DPI) and 2x (192 DPI)
        m_dark_1x = run_case(
            "modern-dark-1x",
            "*appearanceProfile: modern-dark\n*appearanceDensity: comfortable\nXft.dpi: 96\n",
            [
                "--profile",
                "modern-dark",
                "--expect-active-bg",
                "#263248",
                "--expect-inactive-bg",
                "#181B20",
                "--expect-active-border",
                "#4C8DFF",
            ],
        )
        m_dark_2x = run_case(
            "modern-dark-2x",
            "*appearanceProfile: modern-dark\n*appearanceDensity: comfortable\nXft.dpi: 192\n",
            [
                "--profile",
                "modern-dark",
                "--expect-active-bg",
                "#263248",
                "--expect-inactive-bg",
                "#181B20",
                "--expect-active-border",
                "#4C8DFF",
            ],
        )

        # 6. High-contrast at 1x (96 DPI) and 2x (192 DPI)
        m_hc_1x = run_case(
            "high-contrast-1x",
            "*appearanceProfile: high-contrast\n*appearanceDensity: comfortable\nXft.dpi: 96\n",
            [
                "--profile",
                "high-contrast",
                "--expect-active-bg",
                "#0038A8",
                "--expect-inactive-bg",
                "#FFFFFF",
                "--expect-active-border",
                "#000000",
            ],
        )
        m_hc_2x = run_case(
            "high-contrast-2x",
            "*appearanceProfile: high-contrast\n*appearanceDensity: comfortable\nXft.dpi: 192\n",
            [
                "--profile",
                "high-contrast",
                "--expect-active-bg",
                "#0038A8",
                "--expect-inactive-bg",
                "#FFFFFF",
                "--expect-active-border",
                "#000000",
            ],
        )

        # 7. Explicit Xrm color override (Mwm*client*activeBackground)
        run_case(
            "override-active-bg",
            "*appearanceProfile: modern-light\n*appearanceDensity: comfortable\n"
            "Xft.dpi: 96\nMwm*client*activeBackground: #CC3300\n",
            [
                "--profile",
                "modern-light",
                "--expect-active-bg",
                "#CC3300",
                "--expect-inactive-bg",
                "#E8ECF2",
            ],
        )

        # 8. Explicit Mwm*clientDecoration override (border only, no titlebar)
        run_case(
            "override-client-decor",
            "*appearanceProfile: modern-light\n*appearanceDensity: comfortable\n"
            "Xft.dpi: 96\nMwm*clientDecoration: border\n",
            ["--profile", "modern-light", "--expect-no-title"],
        )

        # Verify DPI and density geometry relationships
        if not (
            int(m_light_compact["resize_border"]) < int(m_light_1x["resize_border"])
            and int(m_light_compact["title_height"]) < int(m_light_1x["title_height"])
        ):
            raise RuntimeError(
                f"compact density did not tighten frame geometry: compact={m_light_compact} vs comfortable={m_light_1x}"
            )

        for label, m1, m2 in (
            ("modern-light", m_light_1x, m_light_2x),
            ("modern-dark", m_dark_1x, m_dark_2x),
            ("high-contrast", m_hc_1x, m_hc_2x),
        ):
            if not (
                int(m2["resize_border"]) > int(m1["resize_border"])
                and int(m2["title_height"]) > int(m1["title_height"])
            ):
                raise RuntimeError(
                    f"2x DPI did not scale {label} frame geometry: 1x={m1} vs 2x={m2}"
                )

        if sha256(library) != receipt["library_sha256"]:
            raise RuntimeError("libXm changed during qualification")
        if sha256(mwm_bin) != receipt["mwm_sha256"]:
            raise RuntimeError("mwm binary changed during qualification")

        receipt["success"] = True
        print(
            "PASS WP-48 MWM modern frame appearance, DPI/density scaling, and resource overrides"
        )
        return 0
    finally:
        if write_fd >= 0:
            os.close(write_fd)
        if read_fd >= 0:
            os.close(read_fd)
        if server is not None and server.poll() is None:
            server.terminate()
            try:
                server.wait(timeout=5)
            except subprocess.TimeoutExpired:
                server.kill()
                server.wait()
        receipt["ended_utc"] = datetime.datetime.now(datetime.timezone.utc).isoformat()
        receipt["seconds"] = round(time.monotonic() - started, 3)
        (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n")


if __name__ == "__main__":
    raise SystemExit(main())
