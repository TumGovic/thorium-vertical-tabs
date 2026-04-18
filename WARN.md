Temporary build warnings for this repo.

1. The GitHub Actions Windows workflow uses `win_scripts/write_gn_args.py --mode ci`.
   - It disables Chrome PGO, V8 builtins PGO, and ThinLTO for the remote test
     build.
   - Reason: GitHub CI is being used to produce a runnable validation build for
     vertical-tabs work, not a polished release build.
   - Proper fix later:
     - restore matching Chromium/V8 PGO profiles for the exact tree revision;
     - re-enable ThinLTO and the release-grade optimization path in a dedicated
       release workflow.

2. `src/build/toolchain/win/setup_toolchain.py` appends ATL/MFC include and
   library paths when they exist next to the selected MSVC toolset.
   - Reason: some Windows builders expose ATL/MFC separately and Chromium's
     generated toolchain can miss them, which breaks `atldef.h` resolution.
   - Proper fix later:
     - standardize the Visual Studio layout on every builder and verify the
       expected ATL/MFC files are already wired into the toolchain.

3. This workflow is meant for a larger Windows runner or a self-hosted Windows
   runner.
   - Reason: a full Thorium/Chromium checkout plus build is too large for the
     usual small hosted runner shape.
