//
//  GuiLooperMode.cpp
//  AlphaSoft
//
//  Created by Liam Meredith-Lacey on 25/09/2011.
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

#include "GuiLooperMode.h"
#include "../../File and Settings/AppSettings.h"
#include "../Binary Data/ImageSliderBinaryData.h"
#include "GlobalValues.h"
#include "MainComponent.h"

#define PAD_SETTINGS AppSettings::Instance()->padSettings[currentlySelectedPad]
#define PAD_SETTINGS_i AppSettings::Instance()->padSettings[i]

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

GuiLooperMode::GuiLooperMode(MainComponent &ref)
: mainComponentRef(ref)
										
{
    currentlySelectedPad = 99;
    
    //create circle background graphic
    addAndMakeVisible(circleBackgroundRight = new GuiCircleBackground());
    
    fileChooser = new FilenameComponent ("audiofile",
										 File::nonexistent,
										 false, false, false,
										 "*.wav; *.aif; *.aiff",
										 String::empty,
										 "(choose a WAV or AIFF file)");
	fileChooser->addListener (this);					
	fileChooser->setBrowseButtonText ("+");
	fileChooser->setMaxNumberOfRecentFiles (20);
	addAndMakeVisible (fileChooser);
    fileChooser->addMouseListener(this, true);
    
    //create gain slider
    addAndMakeVisible(gainSlider = new AlphaImageKnob(2));
    gainSlider->sliderComponent()->setRange(0.0, 2.0);
    gainSlider->sliderComponent()->addListener(this);
	gainSlider->sliderComponent()->setValue(0.7, false);
    gainSlider->sliderComponent()->addMouseListener(this, true);
	
	//create pan slider
    addAndMakeVisible(panSlider = new AlphaImageKnob(2, true));
    panSlider->sliderComponent()->setRange(0.0, 1.0);
    panSlider->sliderComponent()->addListener(this);
    panSlider->sliderComponent()->setValue(0.5, false);
    panSlider->sliderComponent()->addMouseListener(this, true);

    
    addAndMakeVisible(playStateMenu = new ComboBox());
    playStateMenu->addListener(this);
    playStateMenu->addItem("Play Once", 1);
    playStateMenu->addItem("Play Once (Continuous)", 2);
    playStateMenu->addItem("Play Once (Non-Destructive)", 21);
    playStateMenu->addItem("Loop", 3);
    playStateMenu->addItem("Toggle (Press-Off)", 4);
    playStateMenu->addItem("Toggle (Release-Off)", 5);
    playStateMenu->addItem("Toggle (Non-Destructive)", 25);
    playStateMenu->addItem("Sticky", 6);
    playStateMenu->addItem("Latch (Max-Latch)", 7);
    playStateMenu->addItem("Latch (Press-Latch)", 8);
    playStateMenu->addItem("Trigger (Looped)", 9);
    playStateMenu->addItem("Trigger (Looped, Non-Destructive)", 22);
    playStateMenu->addItem("Trigger (Non-Looped)", 10);
    playStateMenu->addItem("Trigger (Non-Looped, Non-Destructive)", 24);
    
    playStateMenu->setSelectedId(3, true);
    playStateMenu->addMouseListener(this, true);
    
    //channel slider - this feature is now referenced to 'groups' not channels
    Image *dialImage = new Image(ImageFileFormat::loadFrom(ImageSliderBinaryData::channeldial_png, ImageSliderBinaryData::channeldial_pngSize));
    addAndMakeVisible(channelSlider = new ImageSlider(dialImage, 16, true));
    channelSlider->setSliderStyle(Slider::Rotary);
    channelSlider->setRotaryParameters(0, M_PI*2, false);
    channelSlider->setRange(1, 16, 1);
    channelSlider->addListener(this);
    channelSlider->addMouseListener(this, true);;

    addAndMakeVisible(fxDial = new GuiFxDial(mainComponentRef));
	fxDial->setInterceptsMouseClicks(false, true);
    fxDial->addMouseListener(this, false);
	
	
	addAndMakeVisible (waveform = new DemoThumbnailComp());
	waveform->setInterceptsMouseClicks(false, false);
    
    addAndMakeVisible(triggerModeMenu = new ComboBox());
    triggerModeMenu->addItem("Free", 1);
    triggerModeMenu->addItem("Quantised", 2);
    triggerModeMenu->setSelectedId(1, true);
    triggerModeMenu->addMouseListener(this, true);
    triggerModeMenu->addListener(this);
    
    addAndMakeVisible(pressureModeMenu = new ComboBox());
    //ideally, we should have an 'off' here instead of having a 'no effect' elsewhere
    pressureModeMenu->addItem("Effect processing", 1);
    pressureModeMenu->addItem("Playback manipulation", 2);
    pressureModeMenu->setSelectedId(1, true);
    pressureModeMenu->addMouseListener(this, true);
    pressureModeMenu->addListener(this);
    
}


GuiLooperMode::~GuiLooperMode()
{
    
    deleteAllChildren();
    
}


void GuiLooperMode::resized()
{
    circleBackgroundRight->setBounds(780, 402, 230, 230);
	
    waveform->setBounds(790, 412, 210, 210);
	
    fileChooser->setBounds(RIGHT_CIRCLE_X, 475, COMPONENT_W, COMPONENT_H);
    
    gainSlider->setBounds(845, 495, 45, 45);
	panSlider->setBounds(900, 495, 45, 45);
        
    playStateMenu->setBounds(RIGHT_CIRCLE_X, 538, COMPONENT_W, COMPONENT_H);
    
    channelSlider->setBounds(850, 251, 142, 142);
    
    fxDial->setBounds(14, 402, 230, 230);
	
	triggerModeMenu->setBounds(800, 155, 100, 20);
    
	//gainSlider->setBounds(965, 150, 45, 45);
	//panSlider->setBounds(965, 200, 45, 45);
    
    //test playback manipulation stuff
    //pressureModeMenu->setBounds(10, getHeight()-60, 50, 20);

    
}




void GuiLooperMode::comboBoxChanged (ComboBox* comboBox)
{
    //This method takes the selected IDs from the combo box and sets it to the correct
    //variable in the relevent padSettings index/es
    
    //==============================================================================
    //play state combobox
    if (comboBox == playStateMenu)
    {
        //if individual pad number is selected
        if(currentlySelectedPad < 99)
        {
            //store the value of this combo box in the pad settings of that pad
            PAD_SETTINGS->setLooperPlayState(playStateMenu->getSelectedId());
        }
        
        //if 'all pads' selected
        if(currentlySelectedPad == 99)
        {
            for(int i = 0; i <= 47; i++)
            {
                PAD_SETTINGS_i->setLooperPlayState(playStateMenu->getSelectedId());
            }
        }
        
        //if a 'row' is selected
        if(currentlySelectedPad > 99)
        {
            int row = currentlySelectedPad - 99; 
            
            for(int i = (row*8)-8; i <= (row*8)-1; i++) 
            {
                //i = pad number
                PAD_SETTINGS_i->setLooperPlayState(playStateMenu->getSelectedId());
            }
        }
    }
    
    
    //==============================================================================
    //trigger mode combobox
    if (comboBox == triggerModeMenu)
    {
        //if individual pad number is selected
        if(currentlySelectedPad < 99)
        {
            //store the value of this combo box in the pad settings of that pad
            PAD_SETTINGS->setLooperTriggerMode(triggerModeMenu->getSelectedId());
        }
        
        //if 'all pads' selected
        if(currentlySelectedPad == 99)
        {
            for(int i = 0; i <= 47; i++)
            {
                PAD_SETTINGS_i->setLooperTriggerMode(triggerModeMenu->getSelectedId());
            }
        }
        
        //if a 'row' is selected
        if(currentlySelectedPad > 99)
        {
            int row = currentlySelectedPad - 99; 
            
            for(int i = (row*8)-8; i <= (row*8)-1; i++) 
            {
                //i = pad number
                PAD_SETTINGS_i->setLooperTriggerMode(triggerModeMenu->getSelectedId());
            }
        }
    }
    
    //==============================================================================
    //pressure mode combobox
    if (comboBox == pressureModeMenu)
    {
        //if individual pad number is selected
        if(currentlySelectedPad < 99)
        {
            //store the value of this combo box in the pad settings of that pad
            //PAD_SETTINGS->setLooperTriggerMode(triggerModeMenu->getSelectedId());
        }
        
        //if 'all pads' selected
        if(currentlySelectedPad == 99)
        {
            for(int i = 0; i <= 47; i++)
            {
                //PAD_SETTINGS_i->setLooperTriggerMode(triggerModeMenu->getSelectedId());
            }
        }
        
        //if a 'row' is selected
        if(currentlySelectedPad > 99)
        {
            int row = currentlySelectedPad - 99; 
            
            for(int i = (row*8)-8; i <= (row*8)-1; i++) 
            {
                //i = pad number
                //PAD_SETTINGS_i->setLooperTriggerMode(triggerModeMenu->getSelectedId());
            }
        }
        
        if (pressureModeMenu->getSelectedId() == 1) //DSP effects
        {
            fxDial->setVisible(true);
        }
        else if (pressureModeMenu->getSelectedId() == 2) //playback manipulation
        {
            fxDial->setVisible(false);
        }
    }


}


void GuiLooperMode::sliderValueChanged (Slider* slider)
{
    //==============================================================================
    //gain slider
    if (slider == gainSlider->sliderComponent())
    {
        //if individual pad number is selected
        if(currentlySelectedPad < 99)
        {
            //store the value of this slider in the pad settings of that pad
            PAD_SETTINGS->setLooperGain(gainSlider->sliderComponent()->getValue());
        }
        
        //if 'all pads' selected
        if(currentlySelectedPad == 99)
        {
            for(int i = 0; i <= 47; i++)
            {
                PAD_SETTINGS_i->setLooperGain(gainSlider->sliderComponent()->getValue());            
            }
        }
        
        //if a 'row' is selected
        if(currentlySelectedPad > 99)
        {
            int row = currentlySelectedPad - 99; 
            
            for(int i = (row*8)-8; i <= (row*8)-1; i++) 
            {
                //i = pad number
                PAD_SETTINGS_i->setLooperGain(gainSlider->sliderComponent()->getValue());
            }
        }
    }
    
    //==============================================================================
    //pan slider
    if (slider == panSlider->sliderComponent())
    {
        //if individual pad number is selected
        if(currentlySelectedPad < 99)
        {
            //store the value of this slider in the pad settings of that pad
            PAD_SETTINGS->setLooperPan(panSlider->sliderComponent()->getValue());
        }
        
        //if 'all pads' selected
        if(currentlySelectedPad == 99)
        {
            for(int i = 0; i <= 47; i++)
            {
                PAD_SETTINGS_i->setLooperPan(panSlider->sliderComponent()->getValue());            
            }
        }
        
        //if a 'row' is selected
        if(currentlySelectedPad > 99)
        {
            int row = currentlySelectedPad - 99; 
            
            for(int i = (row*8)-8; i <= (row*8)-1; i++) 
            {
                //i = pad number
                PAD_SETTINGS_i->setLooperPan(panSlider->sliderComponent()->getValue());
            }
        }
        
    }
        //==============================================================================
        //channel slider
        if (slider == channelSlider)
        {
            //if individual pad number is selected
            if(currentlySelectedPad < 99)
            {
                //store the value of this slider in the pad settings of that pad
                PAD_SETTINGS->setLooperChannel(channelSlider->getValue());
            }
            
            //if 'all pads' selected
            if(currentlySelectedPad == 99)
            {
                for(int i = 0; i <= 47; i++)
                {
                    PAD_SETTINGS_i->setLooperChannel(channelSlider->getValue());            
                }
            }
            
            //if a 'row' is selected
            if(currentlySelectedPad > 99)
            {
                int row = currentlySelectedPad - 99; 
                
                for(int i = (row*8)-8; i <= (row*8)-1; i++) 
                {
                    //i = pad number
                    PAD_SETTINGS_i->setLooperChannel(channelSlider->getValue());
                }
            }

        }

}


void GuiLooperMode::filenameComponentChanged (FilenameComponent* filenameComponent)
{
    if (filenameComponent == fileChooser)
    {
        // this is called when the user changes the filename in the file chooser box
        File audioFile (filenameComponent->getCurrentFile().getFullPathName());
        
        if(audioFile.existsAsFile())
        {
            //if individual pad number is selected
            if(currentlySelectedPad < 99)
            {
                //store the file into the correct padSettings index
                PAD_SETTINGS->setLooperAudioFilePath(audioFile);
				
				waveform->setFile (audioFile);
				
            }
            
            //if 'all pads' selected
            if(currentlySelectedPad == 99)
            {
                for(int i = 0; i <= 47; i++)
                {
                    PAD_SETTINGS_i->setLooperAudioFilePath(audioFile); 
					
					waveform->setFile (audioFile);
                }
            }
            
            //if a 'row' is selected
            if(currentlySelectedPad > 99)
            {
                int row = currentlySelectedPad - 99; 
                
                for(int i = (row*8)-8; i <= (row*8)-1; i++) 
                {
                    //i = pad number
                    PAD_SETTINGS_i->setLooperAudioFilePath(audioFile);
					
					waveform->setFile (audioFile);
                }
            }
            
        }
        
        else
        {
            AlertWindow::showMessageBox (AlertWindow::WarningIcon,
                                         "AlphaLive",
                                         "Couldn't open file!\n\n");
        }	
    }
    
}


void GuiLooperMode::setCurrentlySelectedPad (int padNumber)
{
    currentlySelectedPad = padNumber;
    fxDial->setCurrentlySelectedPad(padNumber);
}

void GuiLooperMode::updateDisplay()
{
    //This method is used to set all the components to the currently selected pad's settings,
    //as well as to hide/dissabled any unneeded components. 
    
    //if an individual pad number is currently selected
    if(currentlySelectedPad < 99)
    {
        //fileChooser->setVisible(true);
        //fileChooserLabel->setVisible(true);
        
        fileChooser->setCurrentFile(PAD_SETTINGS->getLooperAudioFilePath(), true, false);
		File audioFile(PAD_SETTINGS->getLooperAudioFilePath());
		waveform->setFile (audioFile);
		
        gainSlider->sliderComponent()->setValue(PAD_SETTINGS->getLooperGain(), false);
        panSlider->sliderComponent()->setValue(PAD_SETTINGS->getLooperPan(), false);
        playStateMenu->setSelectedId(PAD_SETTINGS->getLooperPlayState(), true);
        channelSlider->setValue(PAD_SETTINGS->getLooperChannel(), false);
        triggerModeMenu->setSelectedId(PAD_SETTINGS->getLooperTriggerMode(), true);
        
        
    }
    
    
    //if 'all pads' selected
    if(currentlySelectedPad == 99)
    {
        //fileChooser->setVisible(false);
        //fileChooserLabel->setVisible(false);
        
        //set default values
        fileChooser->setCurrentFile(File::nonexistent, false, false);
        gainSlider->sliderComponent()->setValue(0.7, false);
        panSlider->sliderComponent()->setValue(0.5, false);
        playStateMenu->setSelectedId(3, true);
        channelSlider->setValue(1, false);
        triggerModeMenu->setSelectedId(1, true);
        

        
    }
    
    //if a 'row' is selected
    if(currentlySelectedPad > 99)
    {
        //fileChooser->setVisible(false);
        //fileChooserLabel->setVisible(false);
        
        //set default values
        fileChooser->setCurrentFile(File::nonexistent, false, false);
        gainSlider->sliderComponent()->setValue(0.7, false);
        panSlider->sliderComponent()->setValue(0.5, false);
        playStateMenu->setSelectedId(3, true);
        channelSlider->setValue(1, false);
        triggerModeMenu->setSelectedId(1, true);

                
    }

    fxDial->updateDisplay();
}



void GuiLooperMode::mouseEnter (const MouseEvent &e)
{
    if (channelSlider->isMouseOver(true))
    {
        mainComponentRef.setInfoTextBoxText("Looper Group. Sets and displays the looper group for the selected pad/pads. If the pad is set to a group other than group 1 the loop will be in 'Exclusive Mode' - playing this loop will turn off the previously playing loop of the same group. The default group 1 allows does not enable this exclusive mode.");
    }
    else if (fileChooser->isMouseOver(true))
    {
        mainComponentRef.setInfoTextBoxText("Audio File Selector. Sets and displays the filepath name of the audio file for the selected pad/pads. Use the '+' button to open a File Browser Window, or use the drop-down menu to select from recently selected files, as well as view the currently selected file.");
    }
    else if (panSlider->isMouseOver(true))
    {
        mainComponentRef.setInfoTextBoxText("Pan Knob. Sets and displays the panning/stereo positioning of the selected pad/pads audio signal.");
    }
    else if (gainSlider->isMouseOver(true))
    {
        mainComponentRef.setInfoTextBoxText("Gain Knob. Sets and displays the gain/volume of the selected pad/pads audio signal.");
    }
    else if (playStateMenu->isMouseOver(true))
    {
        mainComponentRef.setInfoTextBoxText("PlayState Drop-down Menu. \nSets and displays the looper PlayState for the selected pad/pads. Looper PlayStates determine how interation with a pad controls the playback and modification of an audio file.");
    }
    else if (fxDial->isMouseOver(true))
    {
        mainComponentRef.setInfoTextBoxText("Looper FX Dial. Sets and displays the audio processing effect that the selected pad/pads pressure controls.");
    }
    else if (triggerModeMenu->isMouseOver(true))
    {
        mainComponentRef.setInfoTextBoxText("Trigger Mode Menu. The trigger mode determines the start time of the loop. 'Free' will trigger the loop as soon as the pad is pressed, 'Quantised' will trigger the loop at the next quantised time value set using the global clock.");
    }
    
}

void GuiLooperMode::mouseExit (const MouseEvent &e)
{
    //remove any text
    mainComponentRef.setInfoTextBoxText (String::empty);
}


