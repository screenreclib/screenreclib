//
// Created by Giulio Carota on 17/10/21.
//

#ifndef SCREENRECLIB_SRVIDEOINPUT_H
#define SCREENRECLIB_SRVIDEOINPUT_H

#include "SRInput.h"

class SRVideoInput: public SRInput{

private:
    int fps;
public:
    SRVideoInput(char *deviceSrc, char *deviceUrl, SRResolution res, int fps);
    AVFormatContext* open() override;
    SRResolution getInputResolution();
};


#endif //SCREENRECLIB_SRVIDEOINPUT_H
