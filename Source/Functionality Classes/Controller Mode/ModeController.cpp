//
//  ModeController.cpp
//  AlphaSoft
//
//  Created by Liam Meredith-Lacey on 21/11/2011.
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

#include "ModeController.h"
#include "../../File and Settings/AppSettings.h"

#define PAD_SETTINGS AppSettings::Instance()->padSettings[padNumber]


ModeController::ModeController(MidiOutput &midiOutput, AlphaLiveEngine &ref)
                    :   midiOutputDevice(&midiOutput),
                        alphaLiveEngineRef(ref)
{
    for (int i = 0; i <= 47; i++)
    {
        prevPadValue[i] = 0;
    }

}

ModeController::~ModeController()
{
    
}


//this function is called from AlphaLiveEngine whenever an OSC message is recieved
//who's pad is set to Controller mode
void ModeController::getInputData(int pad, int value, int velocity)
{
    padNumber = pad;
    padValue = value;
    
    if (PAD_SETTINGS->getControllerControl() == 1) //scene switcher
    {
        changeScene(); 
    }
    else if (PAD_SETTINGS->getControllerControl() == 4) //osc controller
    {
        //should i bet getting the ip addresses and port numbers for each pad beforehand (is it to cpu heavy accessing PadSettings everytime there is an incoming message?)?
        oscOutput.transmitPadMessage(padNumber+1, padValue, velocity, PAD_SETTINGS->getControllerOscIpAddress(), PAD_SETTINGS->getControllerOscPort());
    }
    
    else if (PAD_SETTINGS->getControllerControl() == 2) //MIDI program change
    {
        if (prevPadValue[padNumber] == 0 && padValue > 0)
        {
            MidiMessage message = MidiMessage::programChange(PAD_SETTINGS->getControllerMidiProgramChangeChannel(), PAD_SETTINGS->getControllerMidiProgramChangeNumber());
            sendMidiMessage(message);
        }
    
    }
    
    else if (PAD_SETTINGS->getControllerControl() == 3) //MIDI program change & scene switcher
    {
        if (prevPadValue[padNumber] == 0 && padValue > 0)
        {
            MidiMessage message = MidiMessage::programChange(PAD_SETTINGS->getControllerMidiProgramChangeChannel(), PAD_SETTINGS->getControllerMidiProgramChangeNumber());
            sendMidiMessage(message);
            
            changeScene(); 
        }
        
       
    }

     prevPadValue[padNumber] = padValue;
}


//called when pad is set to the scene switcher control
void ModeController::changeScene()
{
    //instead of using Subject/Observer method here to communicate with SceneComponent,
    //now it would be better to pass in a pointer to MainComponent from AlphaLiveEngine
    //and access SceneComponent that way like recently implemented for the Elite Controls.
    //This would make the Subject/Observer here redundant so I can remove it.
    
    if (prevPadValue[padNumber] == 0 && padValue > 0)
    {
        triggerAsyncUpdate();
        //if there is a 'two clicks' error... it's probably being caused here!
        //how can i fix this?
    }

}


//Output the MIDI messages
void ModeController::sendMidiMessage(MidiMessage midiMessage)
{
    if (alphaLiveEngineRef.getDeviceStatus() != 0)
    {
        unsigned char dataToSend[5];
        
        uint8 *rawMidiMessage = midiMessage.getRawData();
        
        //std::cout << "raw midi message " << std::endl;
        //printf("%02hhx ", rawMidiMessage[0]);
        //printf("%02hhx ", rawMidiMessage[1]);
        //printf("%02hhx ", rawMidiMessage[2]);
        //printf("\n");
        
        dataToSend[0] = 0x00;   //if no reportID's are defined in the descriptor,
                                //must send 0x00. First byte MUST be the report ID.
        dataToSend[1] = MIDI_OUT_COMMAND_ID;
        dataToSend[2] = rawMidiMessage[0]; //midi status byte
        dataToSend[3] = rawMidiMessage[1]; //midi data byte 1
        dataToSend[4] = rawMidiMessage[2]; //midi data byte 2
        
        alphaLiveEngineRef.sendHidControlReport(dataToSend);
    }
    else
    {
        if(midiOutputDevice)
            midiOutputDevice->sendBlockOfMessages(MidiBuffer(midiMessage), Time::getMillisecondCounter(), 44100);
        else
            std::cout << "No MIDI output selected\n";
    }
    
}


void ModeController::setMidiOutputDevice (MidiOutput &midiOutput)
{
    midiOutputDevice = &midiOutput;
}


void ModeController::handleAsyncUpdate()
{
    notifyObs();
}

int ModeController::getPadNumber()
{
    return padNumber;
}
