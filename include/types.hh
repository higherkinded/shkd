#pragma once

#include <cstddef>
#include <cstdlib>
#include <initializer_list>

namespace ty {
    template<typename T> struct array {
        T *data;
        int len;

        array(std::initializer_list<T> elems) {
            data = (T *) std::malloc(sizeof(T) * (elems.size() + 1));
            len  = elems.size();

            int i = 0;
            for (const auto elem : elems) {
                data[i++] = elem;
            }

            data[i] = T();
        }

        array() {
            data = nullptr;
            len = 0;
        }
    };
} // namespace ty
