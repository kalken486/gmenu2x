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
#include "menusettingint.h"
#include "gmenu2x.h"
#include "utilities.h"

#include <sstream>

using std::string;
using std::stringstream;
using fastdelegate::MakeDelegate;

MenuSettingInt::MenuSettingInt(GMenu2X *gmenu2x, const string &name, const string &description, int *value, int min, int max)
	: MenuSetting(gmenu2x,name,description)
{
	IconButton *btn;

	_value = value;
	originalValue = *value;
	this->min = min;
	this->max = max;
	setValue(this->value());

	//Delegates
	ButtonAction actionInc = MakeDelegate(this, &MenuSettingInt::inc);
	ButtonAction actionDec = MakeDelegate(this, &MenuSettingInt::dec);

	btn = new IconButton(gmenu2x, "skin:imgs/buttons/left.png");
	btn->setAction(actionDec);
	buttonBox.add(btn);

	btn = new IconButton(gmenu2x, "skin:imgs/buttons/y.png", gmenu2x->tr["Increase value"]);
	btn->setAction(actionInc);
	buttonBox.add(btn);

	btn = new IconButton(gmenu2x, "skin:imgs/buttons/right.png");
	btn->setAction(actionInc);
	buttonBox.add(btn);

	btn = new IconButton(gmenu2x, "skin:imgs/buttons/x.png", gmenu2x->tr["Decrease value"]);
	btn->setAction(actionDec);
	buttonBox.add(btn);
}

void MenuSettingInt::draw(int y)
{
	MenuSetting::draw(y);
	gmenu2x->s->write( gmenu2x->font, strvalue, 155, y, ASFont::HAlignLeft, ASFont::VAlignTop );
}

void MenuSettingInt::manageInput(bevent_t *event)
{
    switch (event->button) {
        case LEFT:
            dec();
            break;
        case RIGHT:
            inc();
            break;
        case ALTLEFT:
            setValue(value() - 10);
            break;
        case ALTRIGHT:
            setValue(value() + 10);
            break;
        default:
            break;
    }
}

void MenuSettingInt::inc()
{
	setValue(value() + 1);
}

void MenuSettingInt::dec()
{
	setValue(value() - 1);
}

void MenuSettingInt::setValue(int value)
{
	*_value = constrain(value,min,max);
	stringstream ss;
	ss << *_value;
	strvalue = "";
	ss >> strvalue;
}

int MenuSettingInt::value()
{
	return *_value;
}

void MenuSettingInt::adjustInput()
{
#ifdef PLATFORM_GP2X
//	gmenu2x->input.setInterval(30, ACTION_LEFT );
//	gmenu2x->input.setInterval(30, ACTION_RIGHT);
#endif
}

bool MenuSettingInt::edited()
{
	return originalValue != value();
}
