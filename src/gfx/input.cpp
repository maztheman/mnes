#include "input.h"

#include "application.h"

void getKeyStates(std::array<uint32_t, 8>& keyStates)
{
    static Application* pApplication = Application::getApplication();

    for(uint index = 0; auto key : getKeysInOrder())
    {
        keyStates[index++] = static_cast<uint32_t>(pApplication->getKeyState(key));
    }
}
