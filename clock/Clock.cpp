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

#include <Clock.h>
#include <UIObjectFactory.h>
#include <RoundCanvas.h>
#include <Line.h>
#include <Label.h>
#include <ColorPalettes.h>
#include <dtime.h>
#include <GraphicsContext.h>

const float Clock::X_CO[] = { 0.0, 0.10453, 0.20791, 0.30901, 0.40673, 0.5, 0.5878, 0.66913 };
const float Clock::Y_CO[] = { 1.0, 0.99452, 0.97815, 0.95105, 0.91354, 0.86602, 0.80901, 0.74314 };

Clock::Clock(upanui::UIRoot& uiRoot) :
  _uiRoot(uiRoot), _csize(uiRoot.width() - (PADDING * 2) - (uiRoot.borderThickness() * 2)), _htomFactor(5.0f / 60),
  _cx(_csize / 2 - BORDER_THICKNESS), _cy(_csize / 2 - BORDER_THICKNESS) {
  const int r = _csize / 2 - BORDER_THICKNESS - LABEL_SPACE;
  populateSteps(_secondSteps, r - 10);
  populateSteps(_minuteSteps, r - 5);
  populateSteps(_hourSteps, r - 20);
  populateSteps(_labels, r + 3);
  initLayout();
}

void Clock::populateSteps(upanui::Point (&steps)[60], const int r) {
  if (r < 0) {
    throw upan::exception(XLOC, "csize (%u) is smaller than minimum length", _csize);
  }

  for(int i = 0; i < X_Y_CO_SIZE; ++i) {
    steps[i] = upanui::Point(roundtoi(X_CO[i] * r), roundtoi(Y_CO[i] * r));

    if (i == 0) {
      steps[15] = upanui::Point(steps[i].y(), steps[i].x());
      steps[30] = upanui::Point(steps[i].x(), -steps[i].y());
      steps[45] = upanui::Point(-steps[i + 15].x(), steps[i + 15].y());
    } else {
      steps[15 - i] = upanui::Point(steps[i].y(), steps[i].x());
      steps[15 + i] = upanui::Point(steps[15 - i].x(), -steps[15 - i].y());
      steps[45 - i] = upanui::Point(-steps[15 - i].x(), -steps[15 - i].y());
      steps[45 + i] = upanui::Point(-steps[15 - i].x(), steps[15 - i].y());
      steps[30 - i] = upanui::Point(steps[i].x(), -steps[i].y());
      steps[30 + i] = upanui::Point(-steps[i].x(), -steps[i].y());
      steps[60 - i] = upanui::Point(-steps[i].x(), steps[i].y());
    }
  }
}

void Clock::PassThroughMouseHandler::onEvent(upanui::UIObject& uiObject, const upanui::MouseEvent& event) {
  const upanui::MouseData &data = event.getData();
  if (data.leftButtonState() == upanui::MouseData::HOLD) {
    uiObject.parent().xy(uiObject.parent().x() + data.deltaX(), uiObject.parent().y() - data.deltaY());
  }
}

void Clock::initLayout() {
  upanui::GraphicsContext::Transaction gcTransaction;
  auto &clockCanvas = upanui::UIObjectFactory::createRoundCanvas(_uiRoot, PADDING, PADDING, _csize, _csize);
  clockCanvas.borderThickness(BORDER_THICKNESS);
  clockCanvas.backgroundColor(0xFFFEF2);
  clockCanvas.borderColor(0x5A3828);

  auto mouseHandler = new PassThroughMouseHandler();
  clockCanvas.registerMouseEventHandler(*mouseHandler);

  _secondHand = &upanui::UIObjectFactory::createLine(clockCanvas, _cx, _cy, _cx + _secondSteps[0].x(), _cy - _secondSteps[0].y(), 2);
  _secondHand->backgroundColor(0x41394C);

  _minuteHand = &upanui::UIObjectFactory::createLine(clockCanvas, _cx, _cy, _cx + _minuteSteps[0].x(), _cy - _minuteSteps[0].y(), 3);
  _minuteHand->backgroundColor(0x524C4C);

  _hourHand = &upanui::UIObjectFactory::createLine(clockCanvas, _cx, _cy, _cx + _hourSteps[0].x(), _cy - _hourSteps[0].y(), 4);
  _hourHand->backgroundColor(0x4B4B4C);

  auto &centerCircle = upanui::UIObjectFactory::createRoundCanvas(clockCanvas, _cx - 8, _cy - 8, 16, 16);
  centerCircle.backgroundColor(0xFADDBD);

  //VGA font --> 1 character is 8 width, 16 height with font size 16
  const int labelSize = 16;
  const int labelHeight = labelSize;
  const int labelWidth1C = 8;
  const int labelWidth2C = labelWidth1C * 2;
  auto fgColor = ColorPalettes::CP16::Get(ColorPalettes::CP16::FG_BLACK);
  for (int i = 0; i < 12; ++i) {
    const int stepIndex = i * 5;
    const int labelWidth = i == 0 || i > 9 ? labelWidth2C : labelWidth1C;
    char buf[3];
    sprintf(buf, "%d", i == 0 ? 12 : i);
    auto &label = upanui::UIObjectFactory::createLabel(clockCanvas,
                                                       _cx + _labels[stepIndex].x() - labelWidth / 2,
                                                       _cy - _labels[stepIndex].y() - labelHeight / 2,
                                                       labelWidth, labelHeight,
                                                       buf, fgColor,
                                                       upanui::usfn::PreloadedFonts::VGA16,
                                                       upanui::usfn::FAMILY_MONOSPACE, upanui::usfn::STYLE_REGULAR,
                                                       labelSize);
    label.backgroundColor(0);
    label.backgroundColorAlpha(0);
  }
}

void Clock::run() {
  try {
    while (true) {
      RTCDateTime dateTime;
      dtime(&dateTime);

      _secondHand->updateXY(_cx, _cy, _cx + _secondSteps[dateTime._second].x(), _cy - _secondSteps[dateTime._second].y());
      _minuteHand->updateXY(_cx, _cy, _cx + _minuteSteps[dateTime._minute].x(), _cy - _minuteSteps[dateTime._minute].y());

      const int h = (dateTime._hour * 5 + int(dateTime._minute * _htomFactor)) % 60;
      _hourHand->updateXY(_cx, _cy, _cx + _hourSteps[h].x(), _cy - _hourSteps[h].y());

      sleepms(1000);
    }
  } catch (const upan::exception &e) {
    printf("App failed with error: %s", e.ErrorMsg().c_str());
    exit(1);
  }
}