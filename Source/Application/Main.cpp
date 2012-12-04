/*
  ==============================================================================

    This file was auto-generated by the Jucer!

    It contains the basic startup code for a Juce application.

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

#include "../../JuceLibraryCode/JuceHeader.h"
#include "Common.h"
#include "MainWindow.h"
#include "../Functionality Classes/AlphaLiveEngine.h"
#include "../File and Settings/AppSettings.h"
#include "../File and Settings/AppDocumentState.h"
#include "../GUI Classes/Initial Loading Window/LoadingWindow.h"
#include "../File and Settings/StoredSettings.h"
#include "MainMenuModel.h"

//==============================================================================
class AlphaSoftApplication  :   public JUCEApplication
{
public:
    //==============================================================================
    AlphaSoftApplication()
    {
    }

    ~AlphaSoftApplication()
    {
    }

    //==============================================================================
    void initialise (const String& commandLine)
    {
         #if JUCE_MAC || DOXYGEN
         std::cout << "Running on Mac..." << std::endl;
         #endif
         
         #if JUCE_WINDOWS || DOXYGEN
         std::cout << "Running on Windows..." <<std::endl;
         #endif
        
        #if JUCE_LINUX || DOXYGEN
        std::cout << "Running on Linux..." << std::endl;
        #endif
        
        //call this function incase it is the first time the app is run on a computer
        StoredSettings::getInstance()->setDefaultValues();
        
        commandManager = new ApplicationCommandManager();
        commandManager->registerAllCommandsForTarget (this);
         
        // Create temporary loading window. think about how to implement a realtime progress bar in here!
        loadingWindow = new LoadingWindow();
        
        //WOULD IT MAKE MORE SENSE TO DO THE BELOW STUFF IN THE CONSTRUCTOR OF APPDOCUMENTSTATE?
        //delete the current temp directory (incase of previous app crash that wouldn't have deleted the temp file)
        File::getSpecialLocation(File::tempDirectory).deleteRecursively();
        //create temp directory and set to be the current working directory. When a performance is saved,
        //the working directory is then set to the Audio Files folder created alongside the .alphalive file
        File::getSpecialLocation(File::tempDirectory).setAsCurrentWorkingDirectory();
   
        //create a single global instance of AppSettings
        p = AppSettings::Instance();
        
        //all functionality is done within this class!
        alphaLiveEngine = new AlphaLiveEngine();
        AppSettings::Instance()->setAlphaLiveEngineRef(alphaLiveEngine);
        
        //should this be created here? If not, where?
        appDocumentState = new AppDocumentState();
        
        //set menu bar
        menuModel = new MainMenuModel(*appDocumentState);
        
        //create main app window
        mainWindow = new MainAppWindow(*alphaLiveEngine, *appDocumentState, menuModel);
        
        appDocumentState->setMainAppWindowRef (mainWindow);
        
        #if JUCE_MAC
        PopupMenu menu;
        menu.addCommandItem (commandManager, CommandIDs::About);
        menu.addSeparator();
        menu.addCommandItem (commandManager, CommandIDs::Preferences);
            
        MenuBarModel::setMacMainMenu (menuModel, &menu);
        #endif
        
        //delete loading window now as everything will be loaded at this point
        loadingWindow = 0;
        
		//open any requested files/projects
		
		//If the app has been lauched by opening a .alphalive file,
		//get the file path and opent he file.
		//On Mac this seems to be handled within anotherInstanceStarted() only,
		//so this won't currently work on mac here which can cause bugs

		File selectedFile(commandLine.unquoted());
        //check to see if the clicked file is a .alphalive file
        if (selectedFile.getFileExtension() == ".alphalive")
        {
            //load selected file
            appDocumentState->loadProject(false, selectedFile);   
        }
		//#endif
        else if (StoredSettings::getInstance()->launchTask == 2)
        {
            //Open the last project...
            if (StoredSettings::getInstance()->recentFiles.getFile(0) != File::nonexistent)
                appDocumentState->loadProject(false, StoredSettings::getInstance()->recentFiles.getFile(0));
        }
        
        //Tell the HidComms class that it can start recieving and processing pad and elite control reports.
        alphaLiveEngine->setAppHasInitialised();
    }

    
    void shutdown()
    {
        // Do your application's shutdown code here..
        #if JUCE_MAC
        MenuBarModel::setMacMainMenu (nullptr);
        #endif
        menuModel = nullptr;
        delete menuModel;
        
        //delete temp file
        File::getSpecialLocation(File::tempDirectory).deleteRecursively();
        
        delete appDocumentState;
        //change the way the Singleton object gets destroyed?
        delete p;
    
        mainWindow = 0;
        
        delete alphaLiveEngine;
        
        commandManager = nullptr;
        
        StoredSettings::deleteInstance();
    }

    //==============================================================================
    void systemRequestedQuit()
    {
        bool shouldQuit = AlertWindow::showOkCancelBox(AlertWindow::WarningIcon, translate("Quit"), translate("Are you sure you want to quit?"));
        
        if (shouldQuit == true)
        {
            /*
            bool shouldSave = AlertWindow::showOkCancelBox(AlertWindow::WarningIcon, "Save Project", "Would you like to save?", "Yes", "No");
            
            if (shouldSave == true)
            {
                //save
                appDocumentState->savePerformance(0);
            }
             */
            
            //If the option is enabled in the prefs, the project will be automatically cleaned
            //on app shutdown.
            //There is a logical check which compares the number files in the audio files dircetory currently
            //to the number when the project was opened or last cleaned. If they don't match (which most
            //likely means new audio files were added), the project is cleaned.
            if (StoredSettings::getInstance()->cleanOnClose == 2)
            {
                int numOfFilesAtClose = File::getCurrentWorkingDirectory().getNumberOfChildFiles(2);
                
                if (numOfFilesAtClose != appDocumentState->getNumOfFilesAtStart())
                {
                    appDocumentState->removeUneededAudioFiles(true);
                }
            }
            
            quit();
            
        }
        
    }

    //==============================================================================
    const String getApplicationName()
    {
        return "AlphaLive";
    }

    const String getApplicationVersion()
    {
        return ProjectInfo::versionString;
    }

    bool moreThanOneInstanceAllowed()
    {
        return false;
    }

    void anotherInstanceStarted (const String& commandLine)
    {
        //This function gets called anytime the user tries to open a second instance of AlphaLive
        //or when a .alphalive file is opened. commandLine is the file path of the clicked file
        
        File selectedFile(commandLine.unquoted());
        
        //check to see if the clicked file is a .alphalive file
        if (selectedFile.getFileExtension() == ".alphalive")
        {
            //load selected file
            appDocumentState->loadProject(false, selectedFile);
            
        }
    }
    
    //==============================================================================
    //removed the nested MainMenuModel class and put it in a seperate file,
    //due to BAD_ACCESS when trying to call AppDocumentState::loadPerformance
    //from it
    //==============================================================================
    
    
    //==============================================================================
    void getAllCommands (Array <CommandID>& commands)
    {
        //JUCEApplication::getAllCommands (commands);
        
        const CommandID ids[] = 
        {	
            CommandIDs::New,
            CommandIDs::Open,
            CommandIDs::Save,
            CommandIDs::SaveAs,
            CommandIDs::CleanUpProject,
            CommandIDs::updateFirmware
        };
        
        commands.addArray (ids, numElementsInArray (ids));
    }
    
    void getCommandInfo (CommandID commandID, ApplicationCommandInfo& result)
    {
        const int cmd = ModifierKeys::commandModifier;
        const int shift = ModifierKeys::shiftModifier;
        //const int alt = ModifierKeys::altModifier;
        
        //within 'setInfo()' below, the name sets the String that appears in the Menu bar,
        //and the description sets what would appear in the tooltip if the command is set to a button
        //and the tooltip parameter is set to 'true'
        
        if(commandID == CommandIDs::New)
        {
            result.setInfo (translate("New..."),
                            "Creates a new project.",
                            CommandCategories::FileCommands, 0);
            result.defaultKeypresses.add (KeyPress ('n', cmd, 0));
        }
        else if(commandID == CommandIDs::Open)
        {
            result.setInfo (translate("Open..."),
                            "Opens an AlphaLive Project file (.alphalive).",
                            CommandCategories::FileCommands, 0);
            result.defaultKeypresses.add (KeyPress ('o', cmd, 0));
        }
        else if(commandID == CommandIDs::Save)
        {
            result.setInfo (translate("Save"),
                            "Saves the current settings to an AlphaLive Project file (.alphalive).",
                            CommandCategories::FileCommands, 0);
            result.defaultKeypresses.add (KeyPress ('s', cmd, 0));
        }
        else if(commandID == CommandIDs::SaveAs)
        {
            result.setInfo (translate("Save As..."),
                            "Saves the current settings to an AlphaLive Project file (.alphalive).",
                            CommandCategories::FileCommands, 0);
            result.defaultKeypresses.add (KeyPress ('s', cmd|shift, 0));
        }
        else if (commandID == CommandIDs::CleanUpProject)
        {
            result.setInfo (translate("Clean Up Project..."),
                            "Removes any unused audio files from the projects 'Audio Files' directory.",
                            CommandCategories::FileCommands, 0);
        }
        else if (commandID == CommandIDs::updateFirmware)
        {
            result.setInfo (translate("Update Firmware..."),
                            "Updates the firmware on the AlphaSphere device.",
                            CommandCategories::FileCommands, 0);
        }
    }
    
    bool perform (const InvocationInfo& info)
    {
        if(info.commandID == CommandIDs::New)
        {
            appDocumentState->createNewProject();
        }
        else if(info.commandID == CommandIDs::Open)
        {
            appDocumentState->loadProject(true);
        }
        
        else if(info.commandID == CommandIDs::Save)
        {
            appDocumentState->saveProject();
        }
        
        else if(info.commandID == CommandIDs::SaveAs)
        {
            appDocumentState->saveProjectAs();
        }
        
        else if(info.commandID == CommandIDs::CleanUpProject)
        {
            appDocumentState->removeUneededAudioFiles(false);
        }
        
        else if(info.commandID == CommandIDs::updateFirmware)
        {
            /*
             Firmware updater feature.
             Below is the code needed to apply a firmware update to the AlphaSphere.
             It creates a ChildProcess object that runs the bootloader command line app, passing in the
             mmcu type and hex file destination as parameters. It then returns the output as a String.
             The bootloader app and hex file should exist in the Application Data directory.
             
             For now this code is just lauched from a menu bar item and expects the user to manually put the
             device into bootloader mode by pressing the reset button.
             
             Eventually firmware updating should be applied using the following method:
             - The hex file should be numbered to signify the firware version (e.g. SphereWare_1_0.hex, SphereWare_1_1.hex).
             - When the sphere is connected to AlphaLive it sends a report stating its current firmware version.
             - If the included hex file has a greater number than the current firmware version, it pops up
             an alert window telling the user that there is a new firmware version available and asks if they want to update
             it (stressing that they should as otherwise it could limit the softwares functionality - Maybe the user shouldn't
             have an option to not update it?). 
             - The software sends a HID report to the device to change it to the bootloader, an installs the new firmware.
             - The device will then automatically be reconnected and the user can rock out the new firmware. Boo ya. 
             
             Things to consider:
             - Error handling and feedback - make sure all possible errors can be caught and displayed.
             - Where in the AlphaLive code should this be placed when the above method is implemented? 
             - What about if we introduce different firmware versions for different types of foam? We would
             then need an option somewhere for the user to change the firmware, probably within Preferences. 
             
             */
            
            ChildProcess bootloader;
            
            StringArray arguments;
            String appDir(File::getSpecialLocation(File::currentApplicationFile).getParentDirectory().getFullPathName() + File::separatorString);
            
            #if JUCE_MAC
            File bootloaderFile(appDir + "Application Data" + File::separatorString + "firmwareUpdater");
            #endif
            #if JUCE_WINDOWS
            File bootloaderFile(appDir + "Application Data" + File::separatorString + "firmwareUpdater.exe");
            #endif
            //LINUX?
            
            String mmcuString("-mmcu=atmega32u4");
            
            //Get the hexFile. If we start numbering it we will have to find the file differently. 
            File hexFile(appDir + "Application Data" + File::separatorString + "SphereWare.hex");
            
            if (bootloaderFile.exists() == true && hexFile.exists() == true)
            {
                arguments.add(bootloaderFile.getFullPathName());
                arguments.add(mmcuString);
                
                #if JUCE_MAC
                arguments.add(hexFile.getFullPathName());
                #endif
                #if JUCE_WINDOWS
                arguments.add(hexFile.getFullPathName().quoted());  //Needs to be quoted else updaterFirmware.exe
                                                                    //thinks the whole filepath is just from the last space.
                #endif
                //LINUX?
                
                //Send HID report here to change the device to the bootloader.
                //For now, just reconnect the device with the reset button held as per usual
                
                bootloader.start(arguments);
                
                String bootloaderReport =  bootloader.readAllProcessOutput();
                
                std::cout << "..." << bootloaderReport << "..." << std::endl;
                if (bootloaderReport.contains("Unable to open device"))
                {
                    AlertWindow::showMessageBoxAsync (AlertWindow::NoIcon, translate("Cannot Find Device!"), translate("The AlphaSphere does not appear to be connected to the computer. Please connect it and try again."));
                }
                else if (bootloaderReport.isEmpty())
                {
                    AlertWindow::showMessageBoxAsync (AlertWindow::NoIcon, translate("Firmware Updated!"), translate("The AlphaSphere firmware has been successfully updated."));
                }
                else
                {
                    //catch any other outputs (errors most likely)
                    AlertWindow::showMessageBoxAsync (AlertWindow::NoIcon, translate("Error!"), bootloaderReport);
                }
                
            }
            else
            {
                AlertWindow::showMessageBoxAsync (AlertWindow::NoIcon, translate("Missing Files!"), translate("One or more of the files needed to update the firmware are missing. Please consult the FAQ of the reference manual."));
            }
        }
        
        return true;
    }
    
    
   
    //==============================================================================
private:
    ScopedPointer <MainAppWindow> mainWindow;
    ScopedPointer<LoadingWindow> loadingWindow;
    AlphaLiveEngine *alphaLiveEngine;
    AppSettings *p;
    AppDocumentState *appDocumentState;
    
    //ScopedPointer<MainMenuModel> menuModel;
    MainMenuModel *menuModel;
};

//==============================================================================
// This macro generates the main() routine that starts the app.
START_JUCE_APPLICATION(AlphaSoftApplication)

