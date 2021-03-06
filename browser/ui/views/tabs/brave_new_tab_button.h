/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * you can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_VIEWS_TABS_BRAVE_NEW_TAB_BUTTON_H_
#define BRAVE_BROWSER_UI_VIEWS_TABS_BRAVE_NEW_TAB_BUTTON_H_

#include "chrome/browser/ui/views/tabs/new_tab_button.h"

class TabStrip;
namespace views {
  class ButtonListener;
}

class BraveNewTabButton : public NewTabButton {
  public:
    static const gfx::Size kButtonSize;

    using NewTabButton::NewTabButton;

  private:
    gfx::Size CalculatePreferredSize() const override;
    gfx::Path GetBorderPath(const gfx::Point& origin,
                          float scale,
                          bool extend_to_top) const override;
    void PaintPlusIcon(gfx::Canvas* canvas) const override;
    DISALLOW_COPY_AND_ASSIGN(BraveNewTabButton);
};

#endif
