#include "MouseButton.h"

namespace pcg
{
    MouseButtonIndexType GetMouseButtonIndex(MouseButton mouseButton)
    {
        return static_cast<MouseButtonIndexType>(mouseButton);
    }
}
