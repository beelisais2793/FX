/*
  ==============================================================================

    PhiColours.h
    Created: 3 Feb 2021 10:33:51am
    Author:  Alexandre Rodrigues

  ==============================================================================
*/

#pragma once

class PhiColourIds {
    // 256 colours for each class
    enum ColourClass {
        General = 0x000,
        Module = 0x100,
        Connection = 0x200,
        Plug = 0x300,
    };
public:
    struct General {enum Val{
       Background = ColourClass::General,
       TopBar
    };};

    struct Module {enum Val{
        Background = ColourClass::Module,
        OutlineAndText,
        SelectedOutlineAndText,
        HeaderLine,
        Highlight
    };};

    struct Connection {enum Val{
        DefaultFill = ColourClass::Connection,
        SelectedStroke
    };};
    
    // #TODO define Plug colour ids
    struct Plug {enum Val{
        IntletOutline = ColourClass::Plug,
        IntletCenter,
        OutletOutline,
        OutletCenter,
        Text
    };};
};
