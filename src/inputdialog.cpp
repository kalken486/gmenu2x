/***************************************************************************
 *   Copyright (C) 2006 by Massimiliano Torromeo   *
 *   massimiliano.torromeo@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <SDL.h>
#include <SDL_gfxPrimitives.h>

#ifdef TARGET_GP2X
#include "gp2x.h"
#endif
#include "inputdialog.h"

using namespace std;

InputDialog::InputDialog(GMenu2X *gmenu2x, string text, string startvalue) {
	this->gmenu2x = gmenu2x;
	this->text = text;
	input = startvalue;
	selCol = 0;
	selRow = 0;

	// !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~ÀÈÉÌÒÚÄËÏÖÜßàèéìòùäëïöü
	keyboard.resize(5);
	keyboard[0] = "abcdefghijklmnopqrs";
	keyboard[1] = "tuvwxyzàèéìòùäëïöü ";
	keyboard[2] = "0123456789 \"'      ";
	keyboard[3] = "&!?.,:;*+-<=>      ";
	keyboard[4] = "()[]{}|/\\@#$%      ";
}

bool InputDialog::exec() {
	SDL_Rect box = {0, 60, 0, gmenu2x->font->getHeight()+4};

	Uint32 caretTick = 0, curTick;
	bool caretOn = true;

	bool close = false, ok = true;
	while (!close) {
		gmenu2x->sc["imgs/bg.png"]->blit(gmenu2x->s,0,0);
		gmenu2x->writeTitle(text);

		gmenu2x->drawButton(gmenu2x->s, "y", gmenu2x->tr["Shift"],
		gmenu2x->drawButton(gmenu2x->s, "b", gmenu2x->tr["Confirm"],
		gmenu2x->drawButton(gmenu2x->s, "r", gmenu2x->tr["Space"],
		gmenu2x->drawButton(gmenu2x->s, "l", gmenu2x->tr["Backspace"],
		gmenu2x->drawButton(gmenu2x->s, "x", "", 5)-10))));

		box.w = gmenu2x->font->getTextWidth(input)+18;
		box.x = 160-box.w/2;
		gmenu2x->s->box(box.x, box.y, box.w, box.h, gmenu2x->selectionColor);
		gmenu2x->s->rectangle(box.x, box.y, box.w, box.h, gmenu2x->selectionColor);

		gmenu2x->s->write(gmenu2x->font, input, box.x+5, box.y+box.h-2, SFontHAlignLeft, SFontVAlignBottom);

		curTick = SDL_GetTicks();
		if (curTick-caretTick>=600) {
			caretOn = !caretOn;
			caretTick = curTick;
		}

		if (caretOn) gmenu2x->s->box(box.x+box.w-12, box.y+3, 8, box.h-6, gmenu2x->selectionColor);

		drawVirtualKeyboard();

		gmenu2x->s->flip();

#ifdef TARGET_GP2X
		gmenu2x->joy.update();
		// LINK NAVIGATION
		if ( gmenu2x->joy[GP2X_BUTTON_LEFT ] ) selCol--;
		if ( gmenu2x->joy[GP2X_BUTTON_RIGHT] ) selCol++;
		if ( gmenu2x->joy[GP2X_BUTTON_UP   ] ) selRow--;
		if ( gmenu2x->joy[GP2X_BUTTON_DOWN ] ) {
			selRow++;
			if (selRow==(int)keyboard.size()) selCol = selCol<8 ? 0 : 1;
		}
		if ( gmenu2x->joy[GP2X_BUTTON_X] || gmenu2x->joy[GP2X_BUTTON_L] ) {
			//                                      check for utf8 characters
			input = input.substr(0,input.length()-( (unsigned char)input[input.length()-2]==0xc2 || (unsigned char)input[input.length()-2]==0xc3 ? 2 : 1 ));
		}
		if ( gmenu2x->joy[GP2X_BUTTON_R    ] ) input += " ";
		if ( gmenu2x->joy[GP2X_BUTTON_Y    ] ) {
			if (keyboard[0][0]=='A') {
				keyboard[0] = "abcdefghijklmnopqrs";
				keyboard[1] = "tuvwxyzàèéìòùäëïöü ";
			} else {
				keyboard[0] = "ABCDEFGHIJKLMNOPQRS";
				keyboard[1] = "TUVWXYZÀÈÉÌÒÚÄËÏÖÜ ";
			}
		}
		if ( gmenu2x->joy[GP2X_BUTTON_B] || gmenu2x->joy[GP2X_BUTTON_CLICK] ) {
			if (selRow==keyboard.size()) {
				if (selCol==0)
					ok = false;
				close = true;
			} else {
				bool utf8;
				for (uint x=0, xc=0; x<keyboard[selRow].length(); x++) {
					utf8 = (unsigned char)keyboard[selRow][x]==0xc2 || (unsigned char)keyboard[selRow][x]==0xc3;
					if (xc==selCol) input += keyboard[selRow].substr(x, utf8 ? 2 : 1);
					if (utf8) x++;
					xc++;
				}
			}
		}
		if ( gmenu2x->joy[GP2X_BUTTON_START] ) close = true;
#else
		while (SDL_PollEvent(&gmenu2x->event)) {
			if ( gmenu2x->event.type==SDL_KEYDOWN ) {
				if ( gmenu2x->event.key.keysym.sym==SDLK_ESCAPE ) { ok = false; close = true; }
				// LINK NAVIGATION
				if ( gmenu2x->event.key.keysym.sym==SDLK_LEFT      ) selCol--;
				if ( gmenu2x->event.key.keysym.sym==SDLK_RIGHT     ) selCol++;
				if ( gmenu2x->event.key.keysym.sym==SDLK_UP        ) selRow--;
				if ( gmenu2x->event.key.keysym.sym==SDLK_DOWN      )  {
					selRow++;
					if (selRow==(int)keyboard.size()) selCol = selCol<8 ? 0 : 1;
				}
				if ( gmenu2x->event.key.keysym.sym==SDLK_BACKSPACE ) {
					//                                      check for utf8 characters
					input = input.substr(0,input.length()-( (unsigned char)input[input.length()-2]==0xc2 || (unsigned char)input[input.length()-2]==0xc3 ? 2 : 1 ));
				}
				if ( gmenu2x->event.key.keysym.sym==SDLK_LSHIFT    ) {
					if (keyboard[0][0]=='A') {
						keyboard[0] = "abcdefghijklmnopqrs";
						keyboard[1] = "tuvwxyzàèéìòùäëïöü ";
					} else {
						keyboard[0] = "ABCDEFGHIJKLMNOPQRS";
						keyboard[1] = "TUVWXYZÀÈÉÌÒÚÄËÏÖÜ ";
					}
				}
				if ( gmenu2x->event.key.keysym.sym==SDLK_RETURN    ) {
					if (selRow==(int)keyboard.size()) {
						if (selCol==0)
							ok = false;
						close = true;
					} else {
						bool utf8;
						int xc=0;
						for (uint x=0; x<keyboard[selRow].length(); x++) {
							utf8 = (unsigned char)keyboard[selRow][x]==0xc2 || (unsigned char)keyboard[selRow][x]==0xc3;
							if (xc==selCol) input += keyboard[selRow].substr(x, utf8 ? 2 : 1);
							if (utf8) x++;
							xc++;
						}
					}
				}
			}
		}
#endif
	}

	return ok;
}

void InputDialog::drawVirtualKeyboard() {
	//keyboard border
	gmenu2x->s->rectangle(157-keyboard[0].length()*5, 88, keyboard[0].length()*10+4, keyboard.size()*15+18, gmenu2x->selectionColor);
	uint left = 160-keyboard[0].length()*5;

	if (selCol<0) selCol = selRow==(int)keyboard.size() ? 1 : keyboard[0].length()-1;
	if (selCol>=(int)keyboard[0].length()) selCol = 0;
	if (selRow<0) selRow = keyboard.size()-1;
	if (selRow>(int)keyboard.size()) selRow = 0;

	//selection
	if (selRow<(int)keyboard.size())
		gmenu2x->s->box(left+selCol*10-1, 90+selRow*15, 10, 13, gmenu2x->selectionColor);
	else {
		if (selCol>1) selCol = 0;
		if (selCol<0) selCol = 1;
		gmenu2x->s->box(left+selCol*keyboard[0].length()*5-1, 90+keyboard.size()*15, keyboard[0].length()*5, 14, gmenu2x->selectionColor);
	}

	//keys
	for (uint l=0; l<keyboard.size(); l++) {
		string line = keyboard[l];
		for (uint x=0, xc=0; x<line.length(); x++) {
			string charX;
			//utf8 characters
			if ((unsigned char)line[x]==0xc2 || (unsigned char)line[x]==0xc3) {
				charX = line.substr(x,2);
				x++;
			} else
				charX = line[x];
			gmenu2x->s->write(gmenu2x->font, charX, left+4+xc*10, 96+l*15, SFontHAlignCenter, SFontVAlignMiddle);
			xc++;
		}
	}

	//Ok/Cancel
	gmenu2x->s->write(gmenu2x->font, gmenu2x->tr["Cancel"], (int)(160-keyboard[0].length()*2.5), 96+keyboard.size()*15, SFontHAlignCenter, SFontVAlignMiddle);
	gmenu2x->s->write(gmenu2x->font, gmenu2x->tr["OK"], (int)(160+keyboard[0].length()*2.5), 96+keyboard.size()*15, SFontHAlignCenter, SFontVAlignMiddle);
}
