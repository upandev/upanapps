/*
 *  Upanix - An x86 based Operating System
 *  Copyright (C) 2011 'Prajwala Prabhakar' 'srinivasa.prajwal@gmail.com'
 *
 *  I am making my contributions/submissions to this project solely in
 *  my personal capacity and am not conveying any rights to any
 *  intellectual property of any third parties.
 *                                                                          
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *                                                                          
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *                                                                          
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/
 */

#pragma once

#include <UIObject.h>
#include <UIRoot.h>
#include <MouseEventHandler.h>
#include <ithread.h>
#include <Line.h>
#include <UIPosition.h>

class Clock : public upan::thread {
public:
  Clock(upanui::UIRoot&);

private:
  void run() override;
  void initLayout();

  void populateSteps(upanui::UIPosition (&steps)[60], const int r);

  class PassThroughMouseHandler : public upanui::MouseEventHandler {
  public:
    void onEvent(upanui::UIObject& uiObject, const upanui::MouseEvent& event) override;
  };

private:
  upanui::UIRoot& _uiRoot;
  const uint32_t _csize;
  const float _htomFactor;
  const int _cx;
  const int _cy;
  upanui::UIPosition _secondSteps[60];
  upanui::UIPosition _minuteSteps[60];
  upanui::UIPosition _hourSteps[60];
  upanui::UIPosition _labels[60];
  upanui::Line* _secondHand;
  upanui::Line* _minuteHand;
  upanui::Line* _hourHand;

private:
  static const int BORDER_THICKNESS = 5;
  static const int LABEL_SPACE = 20;
  static const int PADDING = 2;
  static const int X_Y_CO_SIZE = 8;
  static const float X_CO[];
  static const float Y_CO[];
};