# Vertical Tabs Plan

`Thorium-Win` is not the right fork target for this feature.
It only hosts Windows builds and packaging docs.

For native Firefox-style vertical tabs, fork `Alex313031/Thorium` and keep
using `Thorium-Win` only for Windows release packaging.

## Repo layout

Thorium is maintained as an overlay on top of a full `chromium/src` checkout.
That means vertical tabs must be developed against a real Chromium tree, then
the changed files should be copied back into the Thorium overlay repo.

Relevant local overlay files already present in this repo:

- `src/chrome/browser/ui/views/tabs/tab_strip.cc`
- `src/chrome/browser/ui/views/frame/browser_root_view.cc`
- `src/chrome/browser/ui/tabs/tab_strip_model.cc`
- `src/chrome/browser/ui/tabs/tab_strip_prefs.cc`
- `src/chrome/browser/ui/tabs/features.cc`
- `src/chrome/browser/thorium_flag_entries.h`
- `src/chrome/browser/about_flags.cc`
- `src/chrome/browser/BUILD.gn`

Important upstream Chromium files that are not currently in the overlay, but
will almost certainly need to be brought in once modified:

- `chrome/browser/ui/views/frame/browser_view.{h,cc}`
- `chrome/browser/ui/views/frame/tab_strip_region_view.{h,cc}`
- `chrome/browser/ui/views/frame/browser_non_client_frame_view*`
- `chrome/browser/ui/views/tabs/tab.{h,cc}`
- `chrome/browser/ui/views/tabs/tab_container*.{h,cc}`
- `chrome/browser/ui/views/tabs/tab_style*.{h,cc}`
- `chrome/browser/ui/views/toolbar/toolbar_view.{h,cc}`

## Scope

This is not a small `TabStripModel` patch.
The current desktop tab strip is built around a horizontal titlebar model:

- top-of-window tab placement
- caption drag/hit-testing
- horizontal tab layout and overflow
- new-tab button placement
- pinned tab compaction
- group headers/underlines
- drag and drop reorder logic
- hover cards and accessibility geometry

## Recommended implementation order

1. Add a Thorium-only experimental flag and pref for vertical tabs.
2. Create a dedicated left-side container view instead of trying to twist the
   existing top `TabStrip` in place.
3. Make `BrowserView` switch between:
   - classic top tab strip
   - left vertical tab sidebar + normal toolbar/content stack
4. Hide top tabstrip hit-testing when vertical mode is enabled.
5. Reuse `TabStripModel` as the data source, but use a separate vertical views
   layout for presentation.
6. Ship an MVP first:
   - activate tab
   - close tab
   - create tab
   - reorder by drag
   - pinned tabs section
7. Add second-wave features only after MVP stabilizes:
   - tab groups
   - collapse/expand behavior
   - hover cards
   - keyboard navigation polish
   - theme metrics

## Practical note

If the goal is "Firefox-like vertical tabs" rather than "rotate the existing
tab strip", a new `VerticalTabSidebar` view is the lower-risk path.
Trying to retrofit the current `TabStrip` directly will fight Chromium's
caption, drag, and layout assumptions in too many places at once.
