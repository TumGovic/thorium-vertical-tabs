// Copyright 2015 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/views/controls/menu/menu_config.h"

#include "ui/base/ui_base_features.h"
#include "ui/ozone/public/ozone_platform.h"

namespace views {

void MenuConfig::InitPlatform() {
  use_bubble_border = corner_radius > 0;
  use_outer_border = true;
  menu_horizontal_border_size = features::IsThorium2024()
                                ? 1
                                : 0;
  submenu_horizontal_overlap = features::IsThorium2024() ? 2 : 0;
  separator_horizontal_border_padding = features::IsThorium2024() ? 5 : 0;
  arrow_to_edge_padding = features::IsThorium2024() ? 0 : 8;
  separator_height = features::IsThorium2024() ? 5 : 13;
  separator_upper_height = features::IsThorium2024() ? 2 : 3;
  separator_lower_height = features::IsThorium2024() ? 2 : 4;
  separator_spacing_height = features::IsThorium2024() ? 2 : 3;
  nonrounded_menu_vertical_border_size = features::IsThorium2024() ? 3 : 4;
}

}  // namespace views
