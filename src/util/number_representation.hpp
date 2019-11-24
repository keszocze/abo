#ifndef NUMBER_REPRESENTATION_HPP
#define NUMBER_REPRESENTATION_HPP

namespace abo::util {

    enum class NumberRepresentation: bool {
        // for unsigned integers
        BaseTwo,
        // for signed integers in twos complement
        TwosComplement
    };

}

#endif // NUMBER_REPRESENTATION_HPP
