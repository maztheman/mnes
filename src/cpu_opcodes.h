#pragma once

constexpr auto OPCODE_ADC_OP = 0x69;
constexpr auto OPCODE_ADC_ZP = 0x65;
constexpr auto OPCODE_ADC_ZP_X = 0x75;
constexpr auto OPCODE_ADC_AB = 0x6D;
constexpr auto OPCODE_ADC_AB_X = 0x7D;
constexpr auto OPCODE_ADC_AB_Y = 0x79;
constexpr auto OPCODE_ADC_IN_X = 0x61;
constexpr auto OPCODE_ADC_IN_Y = 0x71;
#define OPCODE_AND_OP				0x29
#define OPCODE_AND_ZP				0x25
#define OPCODE_AND_ZP_X				0x35
#define OPCODE_AND_AB				0x2D
#define OPCODE_AND_AB_X				0x3D
#define OPCODE_AND_AB_Y				0x39
#define OPCODE_AND_IN_X				0x21
#define OPCODE_AND_IN_Y				0x31
#define OPCODE_ASL_A				0x0A
#define OPCODE_ASL_ZP				0x06
#define OPCODE_ASL_ZP_X				0x16
#define OPCODE_ASL_AB				0x0E
#define OPCODE_ASL_AB_X				0x1E
#define OPCODE_BCC					0x90
#define OPCODE_BCS				0xB0
#define OPCODE_BEQ				0xF0
#define OPCODE_BIT_ZP					0x24
#define OPCODE_BIT_AB					0x2C
#define OPCODE_BMI				0x30
#define OPCODE_BNE				0xD0
#define OPCODE_BPL				0x10
#define OPCODE_BRK				0x00
#define OPCODE_BVC				0x50
#define OPCODE_BVS				0x70
#define OPCODE_CLC				0x18
#define OPCODE_CLD				0xD8
#define OPCODE_CLI				0x58
#define OPCODE_CLV				0xB8
#define OPCODE_CMP_OP				0xC9
#define OPCODE_CMP_ZP				0xC5
#define OPCODE_CMP_ZP_X			0xD5
#define OPCODE_CMP_AB				0xCD
#define OPCODE_CMP_AB_X			0xDD
#define OPCODE_CMP_AB_Y			0xD9
#define OPCODE_CMP_IN_X			0xC1
#define OPCODE_CMP_IN_Y			0xD1
#define OPCODE_CPX_OP				0xE0
#define OPCODE_CPX_ZP				0xE4
#define OPCODE_CPX_AB				0xEC
#define OPCODE_CPY_OP				0xC0
#define OPCODE_CPY_ZP				0xC4
#define OPCODE_CPY_AB				0xCC
#define OPCODE_DEC_ZP					0xC6
#define OPCODE_DEC_ZP_X				0xD6
#define OPCODE_DEC_AB					0xCE
#define OPCODE_DEC_AB_X				0xDE
#define OPCODE_DEX				0xCA
#define OPCODE_DEY				0x88
#define OPCODE_EOR_OP				0x49
#define OPCODE_EOR_ZP				0x45
#define OPCODE_EOR_ZP_X			0x55
#define OPCODE_EOR_AB				0x4D
#define OPCODE_EOR_AB_X			0x5D
#define OPCODE_EOR_AB_Y			0x59
#define OPCODE_EOR_IN_X			0x41
#define OPCODE_EOR_IN_Y			0x51
#define OPCODE_INC_ZP					0xE6
#define OPCODE_INC_ZP_X				0xF6
#define OPCODE_INC_AB					0xEE
#define OPCODE_INC_AB_X				0xFE
#define OPCODE_INX				0xE8
#define OPCODE_INY				0xC8
#define OPCODE_JMP_AB					0x4C
#define OPCODE_JMP_IN					0x6C
#define OPCODE_JSR_AB					0x20
#define OPCODE_LDA_OP				0xA9
#define OPCODE_LDA_ZP				0xA5
#define OPCODE_LDA_ZP_X			0xB5
#define OPCODE_LDA_AB				0xAD
#define OPCODE_LDA_AB_X			0xBD
#define OPCODE_LDA_AB_Y			0xB9
#define OPCODE_LDA_IN_X			0xA1
#define OPCODE_LDA_IN_Y			0xB1
#define OPCODE_LDX_OP				0xA2
#define OPCODE_LDX_ZP				0xA6
#define OPCODE_LDX_ZP_Y			0xB6
#define OPCODE_LDX_AB				0xAE
#define OPCODE_LDX_AB_Y			0xBE
#define OPCODE_LDY_OP				0xA0
#define OPCODE_LDY_ZP				0xA4
#define OPCODE_LDY_ZP_X			0xB4
#define OPCODE_LDY_AB				0xAC
#define OPCODE_LDY_AB_X			0xBC
#define OPCODE_LSR_A				0x4A
#define OPCODE_LSR_ZP				0x46
#define OPCODE_LSR_ZP_X			0x56
#define OPCODE_LSR_AB				0x4E
#define OPCODE_LSR_AB_X			0x5E
#define OPCODE_NOP				0xEA
#define OPCODE_ORA_OP				0x09
#define OPCODE_ORA_ZP				0x05
#define OPCODE_ORA_ZP_X			0x15
#define OPCODE_ORA_AB				0x0D
#define OPCODE_ORA_AB_X			0x1D
#define OPCODE_ORA_AB_Y			0x19
#define OPCODE_ORA_IN_X			0x01
#define OPCODE_ORA_IN_Y			0x11
#define OPCODE_PHA				0x48
#define OPCODE_PHP				0x08
#define OPCODE_PLA				0x68
#define OPCODE_PLP				0x28
#define OPCODE_ROL_A				0x2A
#define OPCODE_ROL_ZP				0x26
#define OPCODE_ROL_ZP_X			0x36
#define OPCODE_ROL_AB				0x2E
#define OPCODE_ROL_AB_X			0x3E
#define OPCODE_ROR_A				0x6A
#define OPCODE_ROR_ZP				0x66
#define OPCODE_ROR_ZP_X			0x76
#define OPCODE_ROR_AB				0x6E
#define OPCODE_ROR_AB_X			0x7E
#define OPCODE_RTI				0x40
#define OPCODE_RTS				0x60
#define OPCODE_SBC_OP				0xE9
#define OPCODE_SBC_IMM_EB			0xEB
#define OPCODE_SBC_ZP				0xE5
#define OPCODE_SBC_ZP_X			0xF5
#define OPCODE_SBC_AB				0xED
#define OPCODE_SBC_AB_X			0xFD
#define OPCODE_SBC_AB_Y			0xF9
#define OPCODE_SBC_IN_X			0xE1
#define OPCODE_SBC_IN_Y			0xF1
#define OPCODE_SEC				0x38
#define OPCODE_SED				0xF8
#define OPCODE_SEI				0x78
#define OPCODE_STA_ZP				0x85
#define OPCODE_STA_ZP_X			0x95
#define OPCODE_STA_AB				0x8D
#define OPCODE_STA_AB_X			0x9D
#define OPCODE_STA_AB_Y			0x99
#define OPCODE_STA_IN_X			0x81
#define OPCODE_STA_IN_Y			0x91
#define OPCODE_STX_ZP				0x86
#define OPCODE_STX_ZP_Y				0x96
#define OPCODE_STX_AB				0x8E
#define OPCODE_STY_ZP				0x84
#define OPCODE_STY_ZP_X			0x94
#define OPCODE_STY_AB				0x8C
#define OPCODE_TAX				0xAA
#define OPCODE_TAY				0xA8
#define OPCODE_TSX				0xBA
#define OPCODE_TXA				0x8A
#define OPCODE_TXS				0x9A
#define OPCODE_TYA				0x98

#define OPCODE_SLO_AB			0x0F
#define OPCODE_SLO_AB_X			0x1F
#define OPCODE_SLO_AB_Y			0x1B
#define OPCODE_SLO_ZP			0x07
#define OPCODE_SLO_ZP_X			0x17
#define OPCODE_SLO_IN_X			0x03
#define OPCODE_SLO_IN_Y			0x13

#define OPCODE_RLA_AB			0x2F
#define OPCODE_RLA_AB_X			0x3F
#define OPCODE_RLA_AB_Y			0x3B
#define OPCODE_RLA_ZP			0x27
#define OPCODE_RLA_ZP_X			0x37
#define OPCODE_RLA_IN_X			0x23
#define OPCODE_RLA_IN_Y			0x33
//aka SRE
#define OPCODE_LSE_AB			0x4F
#define OPCODE_LSE_AB_X			0x5F
#define OPCODE_LSE_AB_Y			0x5B
#define OPCODE_LSE_ZP			0x47
#define OPCODE_LSE_ZP_X			0x57
#define OPCODE_LSE_IN_X			0x43
#define OPCODE_LSE_IN_Y			0x53
#define OPCODE_RRA_AB			0x6F
#define OPCODE_RRA_AB_X			0x7F
#define OPCODE_RRA_AB_Y			0x7B
#define OPCODE_RRA_ZP			0x67
#define OPCODE_RRA_ZP_X			0x77
#define OPCODE_RRA_IN_X			0x63
#define OPCODE_RRA_IN_Y			0x73
#define OPCODE_AXS_AB			0x8F
#define OPCODE_AXS_ZP			0x87
#define OPCODE_AXS_ZP_Y			0x97
#define OPCODE_AXS_IN_X			0x83
#define OPCODE_LAX_AB			0xAF
#define OPCODE_LAX_AB_Y			0xBF
#define OPCODE_LAX_ZP			0xA7
#define OPCODE_LAX_ZP_Y			0xB7
#define OPCODE_LAX_IN_X			0xA3
#define OPCODE_LAX_IN_Y			0xB3
#define OPCODE_DCP_AB			0xCF
#define OPCODE_DCP_AB_X			0xDF
#define OPCODE_DCP_AB_Y			0xDB
#define OPCODE_DCP_ZP			0xC7
#define OPCODE_DCP_ZP_X			0xD7
#define OPCODE_DCP_IN_X			0xC3
#define OPCODE_DCP_IN_Y			0xD3
//aka ISB
#define OPCODE_ISC_AB			0xEF
#define OPCODE_ISC_AB_X			0xFF
#define OPCODE_ISC_AB_Y			0xFB
#define OPCODE_ISC_ZP			0xE7
#define OPCODE_ISC_ZP_X			0xF7
#define OPCODE_ISC_IN_X			0xE3
#define OPCODE_ISC_IN_Y			0xF3
#define OPCODE_ALR_IMM			0x4B
#define OPCODE_ARR_IMM			0x6B
#define OPCODE_XAA_IMM			0x8B
#define OPCODE_OAL_IMM			0xAB
#define OPCODE_AXS_IMM			0xCB
#define OPCODE_NOP_1A			0x1A
#define OPCODE_NOP_3A			0x3A
#define OPCODE_NOP_5A			0x5A
#define OPCODE_NOP_7A			0x7A
#define OPCODE_NOP_DA			0xDA
#define OPCODE_NOP_FA			0xFA
#define OPCODE_NOP_1C_AB_X		0x1C
#define OPCODE_NOP_3C_AB_X		0x3C
#define OPCODE_NOP_5C_AB_X		0x5C
#define OPCODE_NOP_7C_AB_X		0x7C
#define OPCODE_NOP_DC_AB_X		0xDC
#define OPCODE_NOP_FC_AB_X		0xFC
#define OPCODE_NOP_14_ZP_X		0x14
#define OPCODE_NOP_34_ZP_X		0x34
#define OPCODE_NOP_54_ZP_X		0x54
#define OPCODE_NOP_74_ZP_X		0x74
#define OPCODE_NOP_D4_ZP_X		0xD4
#define OPCODE_NOP_F4_ZP_X		0xF4

#define OPCODE_NOP_04_ZP		0x04
#define OPCODE_NOP_44_ZP		0x44
#define OPCODE_NOP_64_ZP		0x64

//missing for sure:
#define OPCODE_SHY_AB_X			0x9C
#define OPCODE_SHX_AB_Y			0x9E

#define OPCODE_TAS_AB_Y			0x9B
#define OPCODE_LAS_AB_Y			0xBB
#define OPCODE_AHX_AB_Y			0x9F
#define OPCODE_AHX_IN_Y			0x93

#define OPCODE_NOP_IMM			0x80
#define OPCODE_NOP_IMM_82		0x82
#define OPCODE_NOP_IMM_89		0x89
#define OPCODE_NOP_IMM_C2		0xC2
#define OPCODE_NOP_IMM_E2		0xE2
#define OPCODE_ANC_IMM_0B		0x0B
#define OPCODE_ANC_IMM_2B		0x2B
#define OPCODE_LAX_IMM			0xAB

#define OPCODE_SAX_ZP			0x87
#define OPCODE_SAX_ZP_Y			0x97

struct SDebugOpCode
{
	const char* sOpCode;
	int nParams;
	int nType;
};

struct SDebugOpCodeFormat
{
	const char* pcszFormat;
};

extern SDebugOpCode OpCodes[256];
extern SDebugOpCodeFormat OpCodeFormats[13];
