//
//  XmlAttributesListBox.cpp
//  AlphaLive
//
//  Created by Liam Meredith-Lacey on 25/08/2012.
//  Copyright 2012 nu desine. 
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

#include "XmlAttributesListBox.h"


XmlAttributesListBox::XmlAttributesListBox (File fileToList)
{
     //will have to check tagname outside this class so it can be used for multiple files.
    //should be check its in an xml format here?
    
    xmlFile = fileToList;
    XmlElement *xmlData = XmlDocument::parse(xmlFile);

    int numOfItems = xmlData->getNumChildElements();
    StringArray itemStrings;
    
    for (int i = 0; i < numOfItems; i++)
        itemStrings.add(xmlData->getChildElement(i)->getStringAttribute("name"));

    listBoxModel = new NoteLayoutListBoxModel(itemStrings);
    addAndMakeVisible(listBox = new ListBox("list box", listBoxModel));

    listBox->setRowHeight(14);
    listBox->updateContent();
  
    delete xmlData;
}

XmlAttributesListBox::~XmlAttributesListBox()
{
    delete listBoxModel;
    deleteAllChildren();
}

void XmlAttributesListBox::resized()
{
    listBox->setBounds(0, 0, getWidth(), getHeight());
}

void XmlAttributesListBox::paint(Graphics& g)
{

}