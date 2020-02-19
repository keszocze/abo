#pragma once

namespace abo::util {

    enum class NumberRepresentation: bool {
        // for unsigned integers
        BaseTwo,
        // for signed integers in twos complement
        TwosComplement
    };

}
