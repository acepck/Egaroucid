/*
    Egaroucid Project

    @file flip_generic.hpp
        Flip calculation without AVX2
    @date 2021-2024
    @author Takuto Yamana
    @license GPL-3.0 license
    @notice I referred to codes written by others
*/

#pragma once
#include "setting.hpp"
#include "common.hpp"
#include "bit.hpp"

const uint64_t bb_vline[8] = {
	0x0101010101010101ull,
	0x0202020202020202ull,
	0x0404040404040404ull,
	0x0808080808080808ull,
	0x1010101010101010ull,
	0x2020202020202020ull,
	0x4040404040404040ull,
	0x8080808080808080ull,
};

const uint64_t bb_mul16[9] = {
	0x0204081020408100ull,
	0x0102040810204080ull,
	0x0081020408102040ull,
	0x0040810204081020ull,
	0x0020408102040810ull,
	0x0010204081020408ull,
	0x0008102040810204ull,
	0x0004081020408102ull,
	0x0002040810204081ull,
};

const uint64_t bb_dline02[64] = {
	0x0000000000000000ull, 0x0000000000000000ull, 0x0000000000000000ull, 0x0000000000000000ull, 0x0000000000000000ull, 0x0000000000000000ull, 0x0000000000000000ull, 0x0000000000000000ull,
	0x0000000000000002ull, 0x0000000000000005ull, 0x000000000000000aull, 0x0000000000000014ull, 0x0000000000000028ull, 0x0000000000000050ull, 0x00000000000000a0ull, 0x0000000000000040ull,
	0x0000000000000204ull, 0x0000000000000508ull, 0x0000000000000a11ull, 0x0000000000001422ull, 0x0000000000002844ull, 0x0000000000005088ull, 0x000000000000a010ull, 0x0000000000004020ull,
	0x0000000000020408ull, 0x0000000000050810ull, 0x00000000000a1120ull, 0x0000000000142241ull, 0x0000000000284482ull, 0x0000000000508804ull, 0x0000000000a01008ull, 0x0000000000402010ull,
	0x0000000002040810ull, 0x0000000005081020ull, 0x000000000a112040ull, 0x0000000014224180ull, 0x0000000028448201ull, 0x0000000050880402ull, 0x00000000a0100804ull, 0x0000000040201008ull,
	0x0000000204081020ull, 0x0000000508102040ull, 0x0000000a11204080ull, 0x0000001422418000ull, 0x0000002844820100ull, 0x0000005088040201ull, 0x000000a010080402ull, 0x0000004020100804ull,
	0x0000020408102040ull, 0x0000050810204080ull, 0x00000a1120408000ull, 0x0000142241800000ull, 0x0000284482010000ull, 0x0000508804020100ull, 0x0000a01008040201ull, 0x0000402010080402ull,
	0x0002040810204080ull, 0x0005081020408000ull, 0x000a112040800000ull, 0x0014224180000000ull, 0x0028448201000000ull, 0x0050880402010000ull, 0x00a0100804020100ull, 0x0040201008040201ull,
};

const uint64_t bb_dline57[64] = {
	0x8040201008040200ull, 0x0080402010080500ull, 0x0000804020110a00ull, 0x0000008041221400ull, 0x0000000182442800ull, 0x0000010204885000ull, 0x000102040810a000ull, 0x0102040810204000ull,
	0x4020100804020000ull, 0x8040201008050000ull, 0x00804020110a0000ull, 0x0000804122140000ull, 0x0000018244280000ull, 0x0001020488500000ull, 0x0102040810a00000ull, 0x0204081020400000ull,
	0x2010080402000000ull, 0x4020100805000000ull, 0x804020110a000000ull, 0x0080412214000000ull, 0x0001824428000000ull, 0x0102048850000000ull, 0x02040810a0000000ull, 0x0408102040000000ull,
	0x1008040200000000ull, 0x2010080500000000ull, 0x4020110a00000000ull, 0x8041221400000000ull, 0x0182442800000000ull, 0x0204885000000000ull, 0x040810a000000000ull, 0x0810204000000000ull,
	0x0804020000000000ull, 0x1008050000000000ull, 0x20110a0000000000ull, 0x4122140000000000ull, 0x8244280000000000ull, 0x0488500000000000ull, 0x0810a00000000000ull, 0x1020400000000000ull,
	0x0402000000000000ull, 0x0805000000000000ull, 0x110a000000000000ull, 0x2214000000000000ull, 0x4428000000000000ull, 0x8850000000000000ull, 0x10a0000000000000ull, 0x2040000000000000ull,
	0x0200000000000000ull, 0x0500000000000000ull, 0x0a00000000000000ull, 0x1400000000000000ull, 0x2800000000000000ull, 0x5000000000000000ull, 0xa000000000000000ull, 0x4000000000000000ull,
	0x0000000000000000ull, 0x0000000000000000ull, 0x0000000000000000ull, 0x0000000000000000ull, 0x0000000000000000ull, 0x0000000000000000ull, 0x0000000000000000ull, 0x0000000000000000ull,
};

const uint8_t bb_seed[64][8] = {
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x08, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00},
	{0x08, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x10, 0x00, 0x04, 0x00, 0x00, 0x00},
	{0x04, 0x00, 0x11, 0x00, 0x04, 0x00, 0x00, 0x00},
	{0x00, 0x10, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00},
	{0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x20, 0x00, 0x08, 0x00, 0x00},
	{0x04, 0x00, 0x01, 0x20, 0x00, 0x08, 0x00, 0x00},
	{0x00, 0x08, 0x00, 0x22, 0x00, 0x08, 0x00, 0x00},
	{0x08, 0x00, 0x00, 0x21, 0x00, 0x08, 0x00, 0x00},
	{0x00, 0x00, 0x20, 0x00, 0x00, 0x04, 0x00, 0x00},
	{0x04, 0x00, 0x21, 0x00, 0x00, 0x04, 0x00, 0x00},
	{0x00, 0x20, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00},
	{0x20, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x10, 0x00},
	{0x04, 0x00, 0x01, 0x00, 0x40, 0x00, 0x10, 0x00},
	{0x00, 0x08, 0x00, 0x02, 0x40, 0x00, 0x10, 0x00},
	{0x08, 0x00, 0x00, 0x01, 0x40, 0x00, 0x10, 0x00},
	{0x00, 0x00, 0x10, 0x00, 0x44, 0x00, 0x10, 0x00},
	{0x04, 0x00, 0x11, 0x00, 0x44, 0x00, 0x10, 0x00},
	{0x00, 0x10, 0x00, 0x00, 0x42, 0x00, 0x10, 0x00},
	{0x10, 0x00, 0x00, 0x00, 0x41, 0x00, 0x10, 0x00},
	{0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x08, 0x00},
	{0x04, 0x00, 0x01, 0x40, 0x00, 0x00, 0x08, 0x00},
	{0x00, 0x08, 0x00, 0x42, 0x00, 0x00, 0x08, 0x00},
	{0x08, 0x00, 0x00, 0x41, 0x00, 0x00, 0x08, 0x00},
	{0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x04, 0x00},
	{0x04, 0x00, 0x41, 0x00, 0x00, 0x00, 0x04, 0x00},
	{0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00},
	{0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x20},
	{0x04, 0x00, 0x01, 0x00, 0x00, 0x80, 0x00, 0x20},
	{0x00, 0x08, 0x00, 0x02, 0x00, 0x80, 0x00, 0x20},
	{0x08, 0x00, 0x00, 0x01, 0x00, 0x80, 0x00, 0x20},
	{0x00, 0x00, 0x10, 0x00, 0x04, 0x80, 0x00, 0x20},
	{0x04, 0x00, 0x11, 0x00, 0x04, 0x80, 0x00, 0x20},
	{0x00, 0x10, 0x00, 0x00, 0x02, 0x80, 0x00, 0x20},
	{0x10, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x20},
	{0x00, 0x00, 0x00, 0x20, 0x00, 0x88, 0x00, 0x20},
	{0x04, 0x00, 0x01, 0x20, 0x00, 0x88, 0x00, 0x20},
	{0x00, 0x08, 0x00, 0x22, 0x00, 0x88, 0x00, 0x20},
	{0x08, 0x00, 0x00, 0x21, 0x00, 0x88, 0x00, 0x20},
	{0x00, 0x00, 0x20, 0x00, 0x00, 0x84, 0x00, 0x20},
	{0x04, 0x00, 0x21, 0x00, 0x00, 0x84, 0x00, 0x20},
	{0x00, 0x20, 0x00, 0x00, 0x00, 0x82, 0x00, 0x20},
	{0x20, 0x00, 0x00, 0x00, 0x00, 0x81, 0x00, 0x20},
	{0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x10},
	{0x04, 0x00, 0x01, 0x00, 0x80, 0x00, 0x00, 0x10},
	{0x00, 0x08, 0x00, 0x02, 0x80, 0x00, 0x00, 0x10},
	{0x08, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x10},
	{0x00, 0x00, 0x10, 0x00, 0x84, 0x00, 0x00, 0x10},
	{0x04, 0x00, 0x11, 0x00, 0x84, 0x00, 0x00, 0x10},
	{0x00, 0x10, 0x00, 0x00, 0x82, 0x00, 0x00, 0x10},
	{0x10, 0x00, 0x00, 0x00, 0x81, 0x00, 0x00, 0x10},
	{0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x08},
	{0x04, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x08},
	{0x00, 0x08, 0x00, 0x82, 0x00, 0x00, 0x00, 0x08},
	{0x08, 0x00, 0x00, 0x81, 0x00, 0x00, 0x00, 0x08},
	{0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x04},
	{0x04, 0x00, 0x81, 0x00, 0x00, 0x00, 0x00, 0x04},
	{0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02},
	{0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01},
};

const uint8_t bb_flipped[137][8] = {
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x02, 0x06, 0x0e, 0x1e, 0x3e, 0x7e},
	{0x00, 0x00, 0x00, 0x04, 0x0c, 0x1c, 0x3c, 0x7c},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x02, 0x00, 0x00, 0x00, 0x08, 0x18, 0x38, 0x78},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x06, 0x04, 0x00, 0x00, 0x00, 0x10, 0x30, 0x70},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x0e, 0x0c, 0x08, 0x00, 0x00, 0x00, 0x20, 0x60},
	{0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x1e, 0x1c, 0x18, 0x10, 0x00, 0x00, 0x00, 0x40},
	{0x00, 0x00, 0x1a, 0x16, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x3e, 0x3c, 0x38, 0x30, 0x20, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x3a, 0x36, 0x2e, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x34, 0x2c, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x7e, 0x7c, 0x78, 0x70, 0x60, 0x40, 0x00, 0x00},
	{0x00, 0x00, 0x7a, 0x76, 0x6e, 0x5e, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x74, 0x6c, 0x5c, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x68, 0x58, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x00, 0x00},
};

const uint64_t bb_h2vline[64] = {
	0x0000000000000000ull, 0x0000000000000100ull, 0x0000000000010000ull, 0x0000000000010100ull,
	0x0000000001000000ull, 0x0000000001000100ull, 0x0000000001010000ull, 0x0000000001010100ull,
	0x0000000100000000ull, 0x0000000000000000ull, 0x0000000100010000ull, 0x0000000100010100ull,
	0x0000000101000000ull, 0x0000000101000100ull, 0x0000000101010000ull, 0x0000000101010100ull,
	0x0000010000000000ull, 0x0000000000000000ull, 0x0000000000000000ull, 0x0000000000000000ull,
	0x0000010001000000ull, 0x0000000000000000ull, 0x0000010001010000ull, 0x0000010001010100ull,
	0x0000010100000000ull, 0x0000000000000000ull, 0x0000010100010000ull, 0x0000010100010100ull,
	0x0000010101000000ull, 0x0000010101000100ull, 0x0000010101010000ull, 0x0000010101010100ull,
	0x0001000000000000ull, 0x0000000000000000ull, 0x0000000000000000ull, 0x0000000000000000ull,
	0x0000000000000000ull, 0x0000000000000000ull, 0x0000000000000000ull, 0x0000000000000000ull,
	0x0001000100000000ull, 0x0000000000000000ull, 0x0000000000000000ull, 0x0000000000000000ull,
	0x0001000101000000ull, 0x0000000000000000ull, 0x0001000101010000ull, 0x0001000101010100ull,
	0x0001010000000000ull, 0x0000000000000000ull, 0x0000000000000000ull, 0x0000000000000000ull,
	0x0001010001000000ull, 0x0000000000000000ull, 0x0001010001010000ull, 0x0001010001010100ull,
	0x0001010100000000ull, 0x0000000000000000ull, 0x0001010100010000ull, 0x0001010100010100ull,
	0x0001010101000000ull, 0x0001010101000100ull, 0x0001010101010000ull, 0x0001010101010100ull,
};

//int_fast8_t n_flip_pre_calc[N_8BIT][HW];

/*
    @brief Flip class

    @param pos                  a cell to put disc
    @param flip                 a bitboard representing flipped discs
*/
class Flip{
    public:
        uint_fast8_t pos;
        uint64_t flip;
    
    public:
        inline uint64_t calc_flip(const uint64_t player, const uint64_t opponent, const int place){
            pos = place;
            uint32_t i = place & 7;
            uint32_t j = place >> 3;

            auto fd = [&](const uint64_t a) {
                uint8_t s = bb_seed[((opponent & a) * bb_vline[1]) >> 58][i];
                s &= ((player & a) * bb_vline[0]) >> 56;
                return (bb_flipped[s][i] * bb_vline[0]) & a;
            };

            uint8_t s16 = (((player >> i) & bb_vline[0]) * bb_mul16[1]) >> 56;
            s16 &= bb_seed[((opponent & bb_vline[i]) * bb_mul16[i]) >> 58][j];
            flip = *((uint64_t *) (((uint32_t *) bb_h2vline) + bb_flipped[s16][j])) << i;

            uint8_t s34 = ((uint8_t (*)[4]) bb_seed)[(opponent >> (place & 0x38)) & 0x7e][i];
            s34 &= player >> (place & 0x38);
            flip |= (uint64_t) bb_flipped[s34][i] << (place & 0x38);

            flip |= fd (bb_dline02[place]);
            flip |= fd (bb_dline57[place]);

            return flip;
        }
};

void flip_init(){
}
