#pragma once

/*
+--------+------+------------------------------------------+
    | Offset | Size | Content(s)                               |
    +--------+------+------------------------------------------+
    |   0    |  3   | 'NES'                                    |
    |   3    |  1   | $1A                                      |
    |   4    |  1   | 16K PRG-ROM page count                   |
    |   5    |  1   | 8K CHR-ROM page count                    |
    |   6    |  1   | ROM Control Byte #1                      |
    |        |      |   %####vTsM                              |
    |        |      |    |  ||||+- 0=Horizontal mirroring      |
    |        |      |    |  ||||   1=Vertical mirroring        |
    |        |      |    |  |||+-- 1=SRAM enabled              |
    |        |      |    |  ||+--- 1=512-byte trainer present  |
    |        |      |    |  |+---- 1=Four-screen mirroring     |
    |        |      |    |  |                                  |
    |        |      |    +--+----- Mapper # (lower 4-bits)     |
    |   7    |  1   | ROM Control Byte #2                      |
    |        |      |   %####0000                              |
    |        |      |    |  |                                  |
    |        |      |    +--+----- Mapper # (upper 4-bits)     |
    |  8-15  |  8   | $00                                      |
    | 16-..  |      | Actual 16K PRG-ROM pages (in linear      |
    |  ...   |      | order). If a trainer exists, it precedes |
    |  ...   |      | the first PRG-ROM page.                  |
    | ..-EOF |      | CHR-ROM pages (in ascending order).      |
    +--------+------+------------------------------------------+
*/

namespace mnes::file {

struct ines_format_t
{
  ines_format_t() = default;

  uint8_t reserved[3];
  uint8_t file_version;
  uint8_t prg_rom_count;
  uint8_t chr_rom_count;
  uint8_t rom_control_1;
  uint8_t rom_control_2;
  uint8_t alignment[8];
};

struct ines_format2
{
  uint8_t reserved[3];
  uint8_t file_version;
  uint8_t prg_rom_count;
  uint8_t chr_rom_count;
  uint8_t rom_control_1;
  uint8_t rom_control_2;
  uint8_t alignment[8];
};

}