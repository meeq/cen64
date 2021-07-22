//
// si/pak.h: Controller pak handling
//
// CEN64: Cycle-Accurate Nintendo 64 Emulator.
// Copyright (C) 2016, Mike Ryan.
//
// This file is subject to the terms and conditions defined in
// 'LICENSE', which is part of this source code package.
//

#ifndef __si_pak_h__
#define __si_pak_h__
#include "common.h"
#include "os/common/rom_file.h"
#include "os/common/save_file.h"
#include "gb.h"

#define MEMPAK_SIZE 0x8000
#define MEMPAK_NUM_PAGES 128
#define MEMPAK_PAGE_SIZE (MEMPAK_SIZE / MEMPAK_NUM_PAGES)

enum controller_type {
  CONTROLLER_NONE     = 0xFFFF,
  CONTROLLER_UNKNOWN  = 0x0000,
  CONTROLLER_STANDARD = 0x0005,
  CONTROLLER_VRU      = 0x0001,
  CONTROLLER_MOUSE    = 0x0200,
  CONTROLLER_KEYBOARD = 0x0002,
  CONTROLLER_TRAIN    = 0x0500,
};

enum pak_type {
  PAK_NONE = 0,
  PAK_MEM,
  PAK_RUMBLE,
  PAK_TRANSFER,
};

struct controller {
  enum controller_type type;

  const char *mempak_path;
  struct save_file mempak_save;

  enum pak_type pak;
  int pak_enabled;
  
  // tpak stuff
  const char *tpak_rom_path;
  struct rom_file tpak_rom;
  const char *tpak_save_path;
  struct save_file tpak_save;
  int tpak_mode;
  int tpak_mode_changed;
  int tpak_bank;
  
  // gb cart stuff
  uint8_t (*gb_readmem [0x100])(struct controller *controller, uint16_t address);
  void    (*gb_writemem[0x100])(struct controller *controller, uint16_t address, uint8_t data);
  struct gb_cart cart;
};

void controller_pak_format(uint8_t *ptr);

int controller_pak_read(struct controller *controller,
    uint8_t *send_buf, uint8_t send_bytes,
    uint8_t *recv_buf, uint8_t recv_bytes);
int controller_pak_write(struct controller *controller,
    uint8_t *send_buf, uint8_t send_bytes,
    uint8_t *recv_buf, uint8_t recv_bytes);

#endif
