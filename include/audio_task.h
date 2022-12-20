#ifndef AUDIO_TASK_H
#define AUDIO_TASK_H

#include "loradv_config.h"

namespace LoraDv {

class AudioTask {

public:
    AudioTask();
    void setup(const Config &config);
};

}

#endif // RADIO_TASK_H