#!/usr/bin/env python3
"""Run the installed-library WP-45 declarative Xrm appearance/density regression."""
import argparse
import os
from pathlib import Path
import platform
import subprocess
import sys


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--prefix", required=True, type=Path)
    parser.add_argument("--out", required=True, type=Path)
    args = parser.parse_args()
    if platform.system() != "Linux":
        parser.error("WP-45 installed Xvfb regression requires Linux/Xvfb")
    prefix = args.prefix.resolve(strict=True)
    output = args.out.resolve()
    if not prefix.is_absolute() or not output.is_absolute() or output.exists():
        parser.error("--out must be a fresh absolute directory")

    source_root = Path(__file__).resolve().parents[2]
    fixture = source_root / "tests/automated/check-xrm-appearance-startup.c"
    output.mkdir(parents=True)

    base_env = dict(
        os.environ,
        LD_LIBRARY_PATH=str(prefix / "lib"),
        LC_ALL="C.UTF-8",
        TMPDIR=str(output),
    )
    for name in ("DISPLAY", "LD_PRELOAD", "LD_AUDIT", "XENVIRONMENT", "XAPPLRESDIR"):
        base_env.pop(name, None)

    binary = output / "check-xrm-appearance-startup"
    compile_command = [
        "cc", "-std=c99", "-Wall", "-Wextra", "-Werror", "-O2",
        "-I" + str(prefix / "include"),
        "-L" + str(prefix / "lib"),
        "-Wl,-rpath," + str(prefix / "lib"),
        str(fixture), "-lXm", "-lXt", "-lX11", "-o", str(binary),
    ]
    compile_log = output / "compile.log"
    with compile_log.open("w") as stream:
        if subprocess.run(compile_command, env=base_env, stdout=stream,
                          stderr=subprocess.STDOUT).returncode:
            print(compile_log.read_text(errors="replace"), file=sys.stderr)
            return 1

    cases = [
        (
            "default-classic",
            "classic",
            "comfortable",
            "*toggle.background: #445566\n",
            [],
        ),
        (
            "xenv-modern-light-underscore",
            "modern-light",
            "comfortable",
            "*appearanceProfile: modern_light\n"
            "*appearanceDensity: comfortable\n"
            "XrmAppearanceConsumer*toggle.background: #445566\n",
            [],
        ),
        (
            "xenv-modern-dark-compact",
            "modern-dark",
            "compact",
            "*appearanceProfile: modern_dark\n"
            "*appearanceDensity: compact\n"
            "XrmAppearanceConsumer*toggle.background: #445566\n",
            [],
        ),
        (
            "xenv-class-high-contrast-compact",
            "high-contrast",
            "compact",
            "XrmAppearanceConsumer.AppearanceProfile: high_contrast\n"
            "XrmAppearanceConsumer.AppearanceDensity: compact\n"
            "XrmAppearanceConsumer*toggle.background: #445566\n",
            [],
        ),
        (
            "xrm-cli-hyphenated",
            "modern-dark",
            "compact",
            "XrmAppearanceConsumer*toggle.background: #445566\n",
            [
                "-xrm", "*appearanceProfile: modern-dark",
                "-xrm", "*appearanceDensity: compact",
            ],
        ),
    ]

    for name, expected_profile, expected_density, xenv_content, cli_args in cases:
        xenv_file = output / f"{name}.Xresources"
        xenv_file.write_text(xenv_content, encoding="utf-8")
        env = dict(
            base_env,
            XENVIRONMENT=str(xenv_file),
            EXPECT_PROFILE=expected_profile,
            EXPECT_DENSITY=expected_density,
        )
        run_log = output / f"{name}.log"
        cmd = [
            "xvfb-run", "-a", "-s", "-noreset -screen 0 1024x768x24",
            str(binary), *cli_args,
        ]
        with run_log.open("w") as stream:
            if subprocess.run(cmd, env=env, stdout=stream,
                              stderr=subprocess.STDOUT).returncode:
                print(run_log.read_text(errors="replace"), file=sys.stderr)
                return 1
        result = run_log.read_text(errors="replace")
        if "PASS WP-45 unmodified Xrm appearance consumer" not in result:
            print(result, file=sys.stderr)
            return 1

    prec_log = output / "precedence.log"
    prec_cmd = [
        "xvfb-run", "-a", "-s", "-noreset -screen 0 1024x768x24",
        str(binary), "--check-precedence",
    ]
    with prec_log.open("w") as stream:
        if subprocess.run(prec_cmd, env=base_env, stdout=stream,
                          stderr=subprocess.STDOUT).returncode:
            print(prec_log.read_text(errors="replace"), file=sys.stderr)
            return 1
    prec_result = prec_log.read_text(errors="replace")
    if "PASS WP-45 Xrm appearance precedence and warning fallback" not in prec_result:
        print(prec_result, file=sys.stderr)
        return 1

    co_log = output / "colorobj.log"
    co_cmd = [
        "xvfb-run", "-a", "-s", "-noreset -screen 0 1024x768x24",
        str(binary), "--check-colorobj",
    ]
    with co_log.open("w") as stream:
        if subprocess.run(co_cmd, env=base_env, stdout=stream,
                          stderr=subprocess.STDOUT).returncode:
            print(co_log.read_text(errors="replace"), file=sys.stderr)
            return 1
    co_result = co_log.read_text(errors="replace")
    if "PASS WP-45 ColorObj and modern appearance profile interop" not in co_result:
        print(co_result, file=sys.stderr)
        return 1

    print("PASS WP-45 declarative Xrm startup appearance and density suite")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
