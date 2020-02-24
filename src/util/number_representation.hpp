#pragma once

namespace abo::util {

    //! This enum represents the number format that BDD forests should be interpreted as
    enum class NumberRepresentation: bool {
        //! used for unsigned integers, the first bit in the vector is the lowest significant one
        BaseTwo,
        //! used for signed integers in twos complement, the first bit in the vector is the lowest significant one
        TwosComplement
    };

}
