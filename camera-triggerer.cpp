/*
	Copyright (C) 2011 Georgia Institute of Technology, University of Utah, Weill Cornell Medical College

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
 * This is a template implementation file for a user module derived from
 * DefaultGUIModel with a custom GUI.
 */

#include "camera-triggerer.h"
#include <main_window.h>
#include <iostream>

namespace Genicam {
   extern const char *START_GENICAM_RECORDING_EVENT;
   extern const char *STOP_GENICAM_RECORDING_EVENT;
	extern const char *GENICAM_SNAPSHOT_EVENT;
}

extern "C" Plugin::Object *createRTXIPlugin(void){
	return new CameraTriggerer();
}

static DefaultGUIModel::variable_t vars[] = {
	{ "GUI label", "Tooltip description", DefaultGUIModel::PARAMETER
		| DefaultGUIModel::DOUBLE, },
	{ "A State", "Tooltip description", DefaultGUIModel::STATE, }, 
};

static size_t num_vars = sizeof(vars) / sizeof(DefaultGUIModel::variable_t);

CameraTriggerer::CameraTriggerer(void) : DefaultGUIModel("Camera Triggerer", ::vars, ::num_vars) {
	setWhatsThis("<p><b>CameraTriggerer:</b><br>Send signals to the GenICam Module that will make it either take a snapshot or toggle recording on or off.</p>");
	DefaultGUIModel::createGUI(vars, num_vars); // this is required to create the GUI
	customizeGUI();
	initParameters();
	update( INIT ); // this is optional, you may place initialization code directly into the constructor
	refresh(); // this is required to update the GUI with parameter and state values
	QTimer::singleShot(0, this, SLOT(resizeMe()));
}

CameraTriggerer::~CameraTriggerer(void) { }

void CameraTriggerer::execute(void) {
	return;
}

void CameraTriggerer::initParameters(void) {
	some_parameter = 0;
	some_state = 0;
}

void CameraTriggerer::update(DefaultGUIModel::update_flags_t flag) {
	switch (flag) {
		case INIT:
			period = RT::System::getInstance()->getPeriod() * 1e-6; // ms
			setParameter("GUI label", some_parameter);
			setState("A State", some_state);
			break;
	
		case MODIFY:
			some_parameter = getParameter("GUI label").toDouble();
			break;

		case UNPAUSE:
			break;

		case PAUSE:
			break;

		case PERIOD:
			period = RT::System::getInstance()->getPeriod() * 1e-6; // ms
			break;
	
		default:
			break;
	}
}

void CameraTriggerer::customizeGUI(void) {
	QGridLayout *customlayout = DefaultGUIModel::getLayout();
	
	QGroupBox *button_group = new QGroupBox;
	
	QPushButton *abutton = new QPushButton("Snap");
	QPushButton *bbutton = new QPushButton("Record");
	bbutton->setCheckable(true);
	QHBoxLayout *button_layout = new QHBoxLayout;
	button_group->setLayout(button_layout);
	button_layout->addWidget(abutton);
	button_layout->addWidget(bbutton);
	QObject::connect(abutton, SIGNAL(clicked()), this, SLOT(aBttn_event()));
	QObject::connect(bbutton, SIGNAL(toggled(bool)), this, SLOT(bBttn_event(bool)));
	
	customlayout->addWidget(button_group, 0,0);
	setLayout(customlayout);
}

// functions designated as Qt slots are implemented as regular C++ functions
void CameraTriggerer::aBttn_event(void) { 
std::cout<<Genicam::GENICAM_SNAPSHOT_EVENT<<std::endl;
	Event::Object event(Genicam::GENICAM_SNAPSHOT_EVENT);
	Event::Manager::getInstance()->postEventRT(&event);
}

void CameraTriggerer::bBttn_event(bool on) { 
	if (on) {
std::cout<<"record on"<<std::endl;
		Event::Object event(Genicam::START_GENICAM_RECORDING_EVENT);
		Event::Manager::getInstance()->postEventRT(&event);
	} else {
std::cout<<"record off"<<std::endl;
		Event::Object event(Genicam::STOP_GENICAM_RECORDING_EVENT);
		Event::Manager::getInstance()->postEventRT(&event);
	}
}
