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
#include <GraphicsContext.h>

int main(int argc, char** argv) {
  Clock* clock = nullptr;
  try {
    const int clockSize = 200;
    const int x = argc > 1 ? atoi(argv[1]) : 0;
    const int y = argc > 2 ? atoi(argv[2]) : 0;

    upanui::GraphicsContext::Init();
    auto& gc = upanui::GraphicsContext::Instance();
    auto& uiRoot = gc.initUIRoot(x, y, clockSize, clockSize, true);
    uiRoot.backgroundColorAlpha(0);

    clock = new Clock(uiRoot);
    clock->start();

    gc.eventManager().startEventLoop();
  } catch(const upan::exception& e) {
    printf("\nError: %s", e.ErrorMsg().c_str());
  }
  if (clock) {
    delete clock;
  }
	return 0 ;
}

