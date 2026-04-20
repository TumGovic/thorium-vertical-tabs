// Copyright 2026 The Chromium Authors and Alex313031
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_VIEWS_FRAME_VERTICAL_TAB_STRIP_CONTAINER_VIEW_H_
#define CHROME_BROWSER_UI_VIEWS_FRAME_VERTICAL_TAB_STRIP_CONTAINER_VIEW_H_

#include "base/memory/raw_ptr.h"
#include "chrome/browser/ui/tabs/tab_strip_model_observer.h"
#include "ui/base/metadata/metadata_header_macros.h"
#include "ui/views/view.h"

class Browser;
class TabStripModel;

namespace content {
class WebContents;
}

namespace views {
class LabelButton;
class ScrollView;
}

class VerticalTabStripContainerView : public views::View,
                                      public TabStripModelObserver {
  METADATA_HEADER(VerticalTabStripContainerView, views::View)

 public:
  static constexpr int kPreferredWidth = 264;

  explicit VerticalTabStripContainerView(Browser* browser);
  VerticalTabStripContainerView(const VerticalTabStripContainerView&) = delete;
  VerticalTabStripContainerView& operator=(
      const VerticalTabStripContainerView&) = delete;
  ~VerticalTabStripContainerView() override;

  // views::View:
  gfx::Size CalculatePreferredSize(
      const views::SizeBounds& available_size) const override;
  gfx::Size GetMinimumSize() const override;
  void OnThemeChanged() override;

  // TabStripModelObserver:
  void OnTabStripModelChanged(TabStripModel* tab_strip_model,
                              const TabStripModelChange& change,
                              const TabStripSelectionChange& selection) override;
  void TabChangedAt(content::WebContents* contents,
                    int index,
                    TabChangeType change_type) override;
  void TabPinnedStateChanged(TabStripModel* tab_strip_model,
                             content::WebContents* contents,
                             int index) override;
  void TabGroupedStateChanged(
      TabStripModel* tab_strip_model,
      std::optional<tab_groups::TabGroupId> old_group,
      std::optional<tab_groups::TabGroupId> new_group,
      tabs::TabInterface* tab,
      int index) override;
  void TabStripEmpty() override;
  void OnTabStripModelDestroyed(TabStripModel* tab_strip_model) override;

 private:
  void ActivateTab(int index);
  void CloseTab(int index);
  std::u16string GetTabTitleForIndex(int index) const;
  void RebuildTabRows();
  void UpdateContainerStyling();

  const raw_ptr<Browser> browser_;
  raw_ptr<TabStripModel> tab_strip_model_ = nullptr;
  raw_ptr<views::LabelButton> new_tab_button_ = nullptr;
  raw_ptr<views::ScrollView> scroll_view_ = nullptr;
  raw_ptr<views::View> rows_container_ = nullptr;
};

#endif  // CHROME_BROWSER_UI_VIEWS_FRAME_VERTICAL_TAB_STRIP_CONTAINER_VIEW_H_
