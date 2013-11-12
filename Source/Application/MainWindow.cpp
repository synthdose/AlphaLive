/*
  ==============================================================================

    This file was auto-generated by the Jucer!

    It contains the basic outline for a simple desktop window.

  ==============================================================================
*/

//  Created by Liam Meredith-Lacey on 14/09/2011.
//  Copyright 2011 nu desine.
//
//  This file is part of AlphaLive.
//
//  AlphaLive is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License, version 2, 
//  as published by the Free Software Foundation.
//  
//  AlphaLive is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "Common.h"
#include "MainWindow.h"

ScopedPointer<ApplicationCommandManager> commandManager;

//==============================================================================
MainAppWindow::MainAppWindow(AlphaLiveEngine &ref, AppDocumentState &ref2, MenuBarModel *menuBar_)
    :               DocumentWindow (JUCEApplication::getInstance()->getApplicationName(),
                    Colours::black,
                    5),
                    alphaLiveEngineRef(ref),
                    appDocumentStateRef(ref2),
                    menuBar(menuBar_)
{
    //create the main component
    mainComponent = new MainComponent(alphaLiveEngineRef, appDocumentStateRef, this);
    
    //see here - http://www.rawmaterialsoftware.com/viewtopic.php?f=3&t=6358&hilit=windows+native+menu+bar
    //I have since abandomed this method, as when moving the apps window around the desktop it would leave
    //'ghost' outlines. So now the TopLevelWindow class is intilialsed with the deafault 'addToDesktop(true)'
    //above, and not done manually below.
    //Though must check this doesn't screw up Windows stuff like described in the forum post.
    
    #if ! JUCE_MAC
    setMenuBar (menuBar);
    #endif
    
    //use native OS title bar
    setUsingNativeTitleBar(true);
    
    //set main component to own the content of the main window
    setContentOwned(mainComponent, false); 
    
    #if JUCE_MAC
    centreWithSize (1024, 690);
    #endif
    #if ! JUCE_MAC
    //add default menu bar height the height to accomadate the menu bar in the overal window
	centreWithSize (1024, 690 + LookAndFeel::getDefaultLookAndFeel().getDefaultMenuBarHeight());
    #endif
    
    // update key mappings.. should this be done here?? What am I even doing here?
    commandManager->getKeyMappings()->resetToDefaultMappings();
    addKeyListener (commandManager->getKeyMappings());
    
    // don't want the window to take focus when the title-bar is clicked..
    setWantsKeyboardFocus (false);
}

MainAppWindow::~MainAppWindow()
{
    #if ! JUCE_MAC
    setMenuBar (nullptr);
    #endif
    delete menuBar;
}

void MainAppWindow::setTitleBarText (String projectName)
{
    setName(JUCEApplication::getInstance()->getApplicationName() + " - " + projectName);
}

void MainAppWindow::closeButtonPressed()
{
    JUCEApplication::getInstance()->systemRequestedQuit();
}


MainComponent* MainAppWindow::getMainComponent()
{
    return mainComponent;
}
