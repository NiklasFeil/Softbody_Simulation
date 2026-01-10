#pragma once

namespace Global {
    void print_error_callback(int num, const char* str);

    namespace Input {
        // When users add new input behaviour, in case the key is already present, they can set whether to
        // 1. overwrite the already assigned callback
        // 2. prepend the new callback to the assigned callback
        // 3. append the new callback to the assigned callback
        enum CallbackBehaviour { OVERWRITE, PREPEND, APPEND };
    }
}