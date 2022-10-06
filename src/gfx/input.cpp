#include "input.h"

#include "platform.h"

std::array<uint32_t, 8> getKeyStates()
{
    static COpenGLWrapper mainframe(CGfxManager::getMainWindow());
    std::array<uint32_t, 8> retval = {0};

    for(uint index = 0; auto key : getKeysInOrder())
    {
        retval[index++] = static_cast<uint32_t>(mainframe.GetKeyState(key));
    }

    return retval;
}