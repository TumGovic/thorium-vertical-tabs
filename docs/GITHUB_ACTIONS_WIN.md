## GitHub Actions Windows Build

This repo now contains a manual GitHub Actions workflow for a remote Windows
test build of Thorium.

Workflow file:

`/.github/workflows/build-thorium-win.yml`

What it does:

- checks out this repo with submodules;
- bootstraps `depot_tools`;
- creates a fresh Chromium checkout from the repo `gclient` spec;
- aligns Chromium to the Thorium version with `win_scripts/version.py`;
- applies the Thorium overlay with `win_scripts/setup.py`;
- writes `out/thorium/args.gn` with `win_scripts/write_gn_args.py`;
- runs `gn gen`;
- builds `thorium` and, by default, `thorium_installer`;
- uploads the resulting installer or build logs as workflow artifacts.

Important notes:

- This is a CI validation build path, not the final release path.
- The workflow defaults to `--mode ci` for `write_gn_args.py`, which disables
  Chrome PGO, V8 builtins PGO, and ThinLTO for a more reliable remote build.
- Use a larger Windows runner or a self-hosted Windows runner. A normal small
  hosted runner is not a realistic target for full Thorium builds.

Recommended manual inputs:

- `runner_labels`:
  - larger/self-hosted Windows runner labels as a JSON array
  - example: `["self-hosted", "Windows", "X64"]`
- `variant`:
  - `avx2` for the current vertical-tabs test build
- `build_installer`:
  - `true` if you want a download-ready installer artifact
