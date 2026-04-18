# Copyright (c) 2026 Alex313031 and OpenAI.

"""
Write out/thorium/args.gn from the repo template in a reproducible way.

This is mainly used by CI so builds do not depend on hand-editing win_args.gn.
"""

import argparse
import os
import pathlib
import re
import sys


REPO_ROOT = pathlib.Path(__file__).resolve().parents[1]


def set_arg(contents, key, value):
    pattern = re.compile(rf"^{re.escape(key)}\s*=.*$", re.MULTILINE)
    replacement = f"{key} = {value}"
    if pattern.search(contents):
        return pattern.sub(replacement, contents)
    return contents.rstrip() + "\n" + replacement + "\n"


def main():
    parser = argparse.ArgumentParser(
        description="Generate out/thorium/args.gn from win_args.gn.")
    parser.add_argument(
        "--variant",
        choices=("default", "avx2"),
        default="default",
        help="Build variant to materialize into args.gn.")
    parser.add_argument(
        "--mode",
        choices=("release", "ci"),
        default="ci",
        help="CI mode disables some expensive or brittle optimizations.")
    args = parser.parse_args()

    cr_src_dir = pathlib.Path(os.getenv("CR_DIR", r"C:/src/chromium/src"))
    args_gn = cr_src_dir / "out" / "thorium" / "args.gn"
    args_gn.parent.mkdir(parents=True, exist_ok=True)

    contents = (REPO_ROOT / "win_args.gn").read_text(encoding="utf-8")

    if args.variant == "avx2":
        variant_args = {
            "use_sse3": "true",
            "use_sse41": "true",
            "use_sse42": "true",
            "use_avx": "true",
            "use_avx2": "true",
            "use_avx512": "false",
            "use_fma": "true",
        }
        for key, value in variant_args.items():
            contents = set_arg(contents, key, value)

    if args.mode == "ci":
        ci_args = {
            "chrome_pgo_phase": "0",
            "pgo_data_path": '""',
            "v8_enable_builtins_optimization": "false",
            "use_thin_lto": "false",
            "thin_lto_enable_optimizations": "false",
        }
        for key, value in ci_args.items():
            contents = set_arg(contents, key, value)

    args_gn.write_text(contents, encoding="utf-8")
    print(f"Wrote {args_gn}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
