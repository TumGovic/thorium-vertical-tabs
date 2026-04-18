// Copyright 2026 The Chromium Authors and Alex313031
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/views/frame/vertical_tab_strip_container_view.h"

#include <memory>
#include <utility>

#include "base/functional/bind.h"
#include "base/strings/utf_string_conversions.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/browser_commands.h"
#include "chrome/browser/ui/tabs/tab_enums.h"
#include "chrome/browser/ui/tabs/tab_strip_model.h"
#include "content/public/browser/web_contents.h"
#include "ui/color/color_id.h"
#include "ui/gfx/geometry/insets.h"
#include "ui/gfx/geometry/size.h"
#include "ui/views/background.h"
#include "ui/views/border.h"
#include "ui/views/controls/button/label_button.h"
#include "ui/views/controls/scroll_view.h"
#include "ui/views/layout/box_layout.h"
#include "ui/views/view.h"

namespace {

constexpr int kButtonCornerRadius = 8;
constexpr int kRowMinHeight = 38;
constexpr int kNewTabButtonHeight = 40;

class VerticalTabRowView : public views::View {
 public:
  VerticalTabRowView(std::u16string title,
                     std::u16string tooltip,
                     bool active,
                     bool closable,
                     base::RepeatingClosure activate_callback,
                     base::RepeatingClosure close_callback)
      : active_(active) {
    auto* layout = SetLayoutManager(std::make_unique<views::BoxLayout>(
        views::BoxLayout::Orientation::kHorizontal));
    layout->set_cross_axis_alignment(
        views::BoxLayout::CrossAxisAlignment::kCenter);
    layout->set_between_child_spacing(6);

    title_button_ = AddChildView(std::make_unique<views::LabelButton>(
        std::move(activate_callback), title));
    title_button_->SetHorizontalAlignment(gfx::ALIGN_LEFT);
    title_button_->SetBorder(
        views::CreateEmptyBorder(gfx::Insets::TLBR(10, 12, 10, 12)));
    title_button_->SetMinSize(gfx::Size(0, kRowMinHeight));
    title_button_->SetTooltipText(tooltip);
    title_button_->SetEnabledTextColors(ui::kColorSysOnSurface);

    layout->SetFlexForView(title_button_, 1, true);

    close_button_ = AddChildView(std::make_unique<views::LabelButton>(
        std::move(close_callback), u"x"));
    close_button_->SetBorder(
        views::CreateEmptyBorder(gfx::Insets::TLBR(10, 10, 10, 10)));
    close_button_->SetMinSize(gfx::Size(30, kRowMinHeight));
    close_button_->SetTooltipText(u"Close tab");
    close_button_->SetVisible(closable);
    close_button_->SetEnabledTextColors(ui::kColorSysOnSurfaceSecondary);

    UpdateStyling();
  }

  VerticalTabRowView(const VerticalTabRowView&) = delete;
  VerticalTabRowView& operator=(const VerticalTabRowView&) = delete;
  ~VerticalTabRowView() override = default;

  // views::View:
  void OnThemeChanged() override {
    views::View::OnThemeChanged();
    UpdateStyling();
  }

 private:
  void UpdateStyling() {
    if (active_) {
      SetBackground(views::CreateRoundedRectBackground(
          ui::kColorSysStateHoverOnSubtle, kButtonCornerRadius));
    } else {
      SetBackground(std::unique_ptr<views::Background>());
    }
  }

  const bool active_;
  raw_ptr<views::LabelButton> title_button_ = nullptr;
  raw_ptr<views::LabelButton> close_button_ = nullptr;
};

std::u16string GetFallbackTitle(content::WebContents* contents) {
  if (!contents) {
    return u"New Tab";
  }

  std::u16string title = contents->GetTitle();
  if (!title.empty()) {
    return title;
  }

  const GURL& visible_url = contents->GetVisibleURL();
  if (!visible_url.is_empty()) {
    return base::UTF8ToUTF16(visible_url.spec());
  }

  return u"New Tab";
}

}  // namespace

VerticalTabStripContainerView::VerticalTabStripContainerView(Browser* browser)
    : browser_(browser) {
  auto* layout = SetLayoutManager(std::make_unique<views::BoxLayout>(
      views::BoxLayout::Orientation::kVertical,
      gfx::Insets::TLBR(8, 8, 8, 8), 8));
  layout->set_cross_axis_alignment(
      views::BoxLayout::CrossAxisAlignment::kStretch);

  new_tab_button_ = AddChildView(
      std::make_unique<views::LabelButton>(
          base::BindRepeating(
              [](Browser* browser) { chrome::NewTab(browser); },
              browser_.get()),
          u"+ New Tab"));
  new_tab_button_->SetHorizontalAlignment(gfx::ALIGN_LEFT);
  new_tab_button_->SetBorder(
      views::CreateEmptyBorder(gfx::Insets::TLBR(10, 12, 10, 12)));
  new_tab_button_->SetMinSize(gfx::Size(0, kNewTabButtonHeight));
  new_tab_button_->SetTooltipText(u"Open a new tab");

  auto scroll_view = std::make_unique<views::ScrollView>();
  scroll_view_ = AddChildView(std::move(scroll_view));
  scroll_view_->SetDrawOverflowIndicator(false);
  scroll_view_->SetHorizontalScrollBarMode(
      views::ScrollView::ScrollBarMode::kDisabled);
  scroll_view_->SetVerticalScrollBarMode(
      views::ScrollView::ScrollBarMode::kEnabled);
  scroll_view_->SetBackgroundColor(ui::kColorSysSurface);
  layout->SetFlexForView(scroll_view_, 1);

  auto rows_container = std::make_unique<views::View>();
  auto* rows_layout = rows_container->SetLayoutManager(
      std::make_unique<views::BoxLayout>(
          views::BoxLayout::Orientation::kVertical));
  rows_layout->set_between_child_spacing(4);
  rows_container_ = scroll_view_->SetContents(std::move(rows_container));

  if (browser_) {
    tab_strip_model_ = browser_->tab_strip_model();
    if (tab_strip_model_) {
      tab_strip_model_->AddObserver(this);
    }
  }

  UpdateContainerStyling();
  RebuildTabRows();
}

VerticalTabStripContainerView::~VerticalTabStripContainerView() {
  if (tab_strip_model_) {
    tab_strip_model_->RemoveObserver(this);
  }
}

gfx::Size VerticalTabStripContainerView::CalculatePreferredSize(
    const views::SizeBounds&) const {
  return gfx::Size(kPreferredWidth, 0);
}

gfx::Size VerticalTabStripContainerView::GetMinimumSize() const {
  return gfx::Size(kPreferredWidth, 0);
}

void VerticalTabStripContainerView::OnThemeChanged() {
  views::View::OnThemeChanged();
  UpdateContainerStyling();
}

void VerticalTabStripContainerView::OnTabStripModelChanged(
    TabStripModel*,
    const TabStripModelChange&,
    const TabStripSelectionChange&) {
  RebuildTabRows();
}

void VerticalTabStripContainerView::TabChangedAt(
    content::WebContents*,
    int,
    TabChangeType) {
  RebuildTabRows();
}

void VerticalTabStripContainerView::TabPinnedStateChanged(
    TabStripModel*,
    content::WebContents*,
    int) {
  RebuildTabRows();
}

void VerticalTabStripContainerView::TabGroupedStateChanged(
    TabStripModel*,
    std::optional<tab_groups::TabGroupId>,
    std::optional<tab_groups::TabGroupId>,
    tabs::TabInterface*,
    int) {
  RebuildTabRows();
}

void VerticalTabStripContainerView::TabStripEmpty() {
  RebuildTabRows();
}

void VerticalTabStripContainerView::OnTabStripModelDestroyed(
    TabStripModel* tab_strip_model) {
  if (tab_strip_model_ == tab_strip_model) {
    tab_strip_model_ = nullptr;
  }
  RebuildTabRows();
}

void VerticalTabStripContainerView::ActivateTab(int index) {
  if (!tab_strip_model_ || !tab_strip_model_->ContainsIndex(index)) {
    return;
  }
  tab_strip_model_->ActivateTabAt(index);
}

void VerticalTabStripContainerView::CloseTab(int index) {
  if (!tab_strip_model_ || !tab_strip_model_->ContainsIndex(index) ||
      !tab_strip_model_->IsTabClosable(index)) {
    return;
  }
  tab_strip_model_->CloseWebContentsAt(
      index, TabCloseTypes::CLOSE_CREATE_HISTORICAL_TAB |
                 TabCloseTypes::CLOSE_USER_GESTURE);
}

std::u16string VerticalTabStripContainerView::GetTabTitleForIndex(
    int index) const {
  if (!tab_strip_model_ || !tab_strip_model_->ContainsIndex(index)) {
    return std::u16string();
  }

  std::u16string title =
      GetFallbackTitle(tab_strip_model_->GetWebContentsAt(index));
  if (tab_strip_model_->IsTabPinned(index)) {
    title.insert(0, u"[P] ");
  }
  return title;
}

void VerticalTabStripContainerView::RebuildTabRows() {
  if (!rows_container_) {
    return;
  }

  rows_container_->RemoveAllChildViews();

  if (!tab_strip_model_) {
    PreferredSizeChanged();
    return;
  }

  for (int index = 0; index < tab_strip_model_->count(); ++index) {
    const std::u16string title = GetTabTitleForIndex(index);
    rows_container_->AddChildView(std::make_unique<VerticalTabRowView>(
        title, title, index == tab_strip_model_->active_index(),
        tab_strip_model_->IsTabClosable(index),
        base::BindRepeating(&VerticalTabStripContainerView::ActivateTab,
                            base::Unretained(this), index),
        base::BindRepeating(&VerticalTabStripContainerView::CloseTab,
                            base::Unretained(this), index)));
  }

  rows_container_->InvalidateLayout();
  scroll_view_->InvalidateLayout();
  PreferredSizeChanged();
}

void VerticalTabStripContainerView::UpdateContainerStyling() {
  SetBackground(views::CreateSolidBackground(ui::kColorSysSurface));
  SetBorder(views::CreateSolidSidedBorder(gfx::Insets::TLBR(0, 0, 0, 1),
                                          ui::kColorSysDivider));

  if (new_tab_button_) {
    new_tab_button_->SetBackground(views::CreateRoundedRectBackground(
        ui::kColorSysSurface2, kButtonCornerRadius));
    new_tab_button_->SetEnabledTextColors(ui::kColorSysOnSurface);
  }
}
