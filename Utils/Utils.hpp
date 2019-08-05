//
//  Utils.hpp
//  SoftDrop
//
//  Created by James Hovet on 8/5/19.
//  Copyright © 2019 James Hovet. All rights reserved.
//

#ifndef Utils_hpp
#define Utils_hpp

#include <stdio.h>

namespace Utils {
    inline static int printable(char byte){
        return (((unsigned short)byte)&0xff);
    }
}

#endif /* Utils_hpp */
