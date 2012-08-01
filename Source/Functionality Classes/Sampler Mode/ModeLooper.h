//
//  ModeLooper.h
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


#ifndef H_MODELOOPER
#define H_MODELOOPER

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "AudioFilePlayer.h"
#include "../../File and Settings/PadSettings.h"

class AlphaLiveEngine;

class ModeLooper :  public AudioSource
{
public:
    //==========================================================================================
    ModeLooper(AlphaLiveEngine &ref);
    ~ModeLooper();
    
    //==========================================================================================
    void createAudioFilePlayer (int padNumber);
    void deleteAudioFilePlayer (int padNumber);
    
    void getOscData(int padNumber, int padValue);
    
    //used by PadSettings to access the AudioFilePlayer instances
    AudioFilePlayer* getAudioFilePlayerInstance (int padNumber);
    
    //AudioSource Functions
	void prepareToPlay (int samplesPerBlockExpected,double sampleRate);
	void releaseResources();
	void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill);
    
    void updatePadPlayingStatus(int padNumber, int playingStatus);
    
    void killPad (int padNum);
    
    //quantization stuff
    void triggerQuantizationPoint (int padNum);
    
    AlphaLiveEngine& getAlphaLiveEngineRef();

private:
    
    OwnedArray<AudioFilePlayer> padLooper;
    
    //==========================================================================================

    //audio related
	MixerAudioSource audioMixer;
	AudioSourcePlayer audioPlayer;
    ScopedPointer <TimeSliceThread> audioTransportSourceThread;
    
    CriticalSection sharedMemory;
    
    int prevPadValue[48];
    
    AlphaLiveEngine &alphaLiveEngineRef;
    
};



#endif