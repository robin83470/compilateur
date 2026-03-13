#pragma once

#include <cstdint>
#include <ostream>
#include <stdexcept>

namespace arm_codegen {

inline void emitLoadWFromOffset(std::ostream& out, int offset, const char* reg, const char* scratchReg = "x9") {
    if (offset >= -256 && offset <= 255) {
        out << "    ldur " << reg << ", [x29, #" << offset << "]\n";
    } else if (offset < 0 && -offset <= 4095) {
        out << "    sub " << scratchReg << ", x29, #" << -offset << "\n";
        out << "    ldr " << reg << ", [" << scratchReg << "]\n";
    } else if (offset > 0 && offset <= 4095) {
        out << "    add " << scratchReg << ", x29, #" << offset << "\n";
        out << "    ldr " << reg << ", [" << scratchReg << "]\n";
    } else {
        throw std::runtime_error("ARM stack offset out of supported range");
    }
}

inline void emitStoreWToOffset(std::ostream& out, int offset, const char* reg, const char* scratchReg = "x9") {
    if (offset >= -256 && offset <= 255) {
        out << "    stur " << reg << ", [x29, #" << offset << "]\n";
    } else if (offset < 0 && -offset <= 4095) {
        out << "    sub " << scratchReg << ", x29, #" << -offset << "\n";
        out << "    str " << reg << ", [" << scratchReg << "]\n";
    } else if (offset > 0 && offset <= 4095) {
        out << "    add " << scratchReg << ", x29, #" << offset << "\n";
        out << "    str " << reg << ", [" << scratchReg << "]\n";
    } else {
        throw std::runtime_error("ARM stack offset out of supported range");
    }
}

inline void emitLoadImm32(std::ostream& out, const char* reg, int value) {
    uint32_t u = static_cast<uint32_t>(value);
    uint16_t lo = static_cast<uint16_t>(u & 0xFFFFu);
    uint16_t hi = static_cast<uint16_t>((u >> 16) & 0xFFFFu);

    out << "    movz " << reg << ", #" << lo << "\n";
    if (hi != 0) {
        out << "    movk " << reg << ", #" << hi << ", lsl #16\n";
    }
}

} // namespace arm_codegen
