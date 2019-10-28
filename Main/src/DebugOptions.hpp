//
//  DebugOptions.h
//  SoftDrop
//
//  Created by James Hovet on 10/24/19.
//  Copyright © 2019 James Hovet. All rights reserved.
//

#ifndef DebugOptions_h
#define DebugOptions_h


/**
 Singleton class for managing all debug options
 */
class DebugOptions {
public:
    bool shouldDrawImGui {true};
    bool shouldDrawSprites {true};
    bool shouldDrawNametables {true};
    //TODO: set false once I actually get around to hooking up the spritetable code
    bool shouldOverrideSpritetable {true};
    int spritesheetOverride {0};
    
    bool isPaused {false};
    bool shouldAdvanceOneFrame{false};
    
    //logging
    bool shouldLogToStdout {true};
    bool shouldPrintFrameNumbers {false};
};

#endif /* DebugOptions_h */
