//
// os/common/input.c: Input device handling functions.
//
// CEN64: Cycle-Accurate Nintendo 64 Emulator.
// Copyright (C) 2015, Tyler J. Stachecki.
//
// This file is subject to the terms and conditions defined in
// 'LICENSE', which is part of this source code package.
//

#include "bus/controller.h"
#include "common.h"
#include "input.h"
#include "os/keycodes.h"
#include "si/controller.h"

bool shift_down;
bool left_down;
bool right_down;
bool up_down;
bool down_down;

uint16_t randnet_keyboard_key_encode(unsigned key) {
  switch (key) {
    // 1st Row
    case CEN64_KEY_ESCAPE:    return 0x0A08;
    case CEN64_KEY_F1:        return 0x0B01;
    case CEN64_KEY_F2:        return 0x0A01; 
    case CEN64_KEY_F3:        return 0x0B08; 
    case CEN64_KEY_F4:        return 0x0A07; 
    case CEN64_KEY_F5:        return 0x0B07; 
    case CEN64_KEY_F6:        return 0x0A02; 
    case CEN64_KEY_F7:        return 0x0B02; 
    case CEN64_KEY_F8:        return 0x0A03; 
    case CEN64_KEY_F9:        return 0x0B03; 
    case CEN64_KEY_F10:       return 0x0A04; 
    case CEN64_KEY_F11:       return 0x0203; 
    case CEN64_KEY_F12:       return 0x0B06;
    case CEN64_KEY_NUMLOCK:   return 0x0A05;
    // case UNKNOWN:          return 0x0B05;
    // case UNKNOWN:          return 0x0208;
    // case UNKNOWN:          return 0x0207;
    // 2nd Row
    // case UNKNOWN:          return 0x0D05;
    case CEN64_KEY_1:         return 0x0C05;
    case CEN64_KEY_2:         return 0x0505;
    case CEN64_KEY_3:         return 0x0605;
    case CEN64_KEY_4:         return 0x0705;
    case CEN64_KEY_5:         return 0x0805;
    case CEN64_KEY_6:         return 0x0905;
    case CEN64_KEY_7:         return 0x0906;
    case CEN64_KEY_8:         return 0x0806;
    case CEN64_KEY_9:         return 0x0706;
    case CEN64_KEY_0:         return 0x0606;
    // case MISMATCH:         return 0x0506;
    // case MISMATCH:         return 0x0C06;
    case CEN64_KEY_BACKSPACE: return 0x0D06;
    // 3rd Row
    case CEN64_KEY_TAB:       return 0x0D01;
    case CEN64_KEY_Q:         return 0x0C01;
    case CEN64_KEY_W:         return 0x0501;
    case CEN64_KEY_E:         return 0x0601;
    case CEN64_KEY_R:         return 0x0701;
    case CEN64_KEY_T:         return 0x0801;
    case CEN64_KEY_Y:         return 0x0901;
    case CEN64_KEY_U:         return 0x0904;
    case CEN64_KEY_I:         return 0x0804;
    case CEN64_KEY_O:         return 0x0704;
    case CEN64_KEY_P:         return 0x0604;
    // case MISMATCH:         return 0x0504;
    // case MISMATCH:         return 0x0C04;
    // case MISMATCH:         return 0x0406;
    // 4th Row
    case CEN64_KEY_CAPSLOCK:  return 0x0F05;
    case CEN64_KEY_A:         return 0x0D07;
    case CEN64_KEY_S:         return 0x0C07;
    case CEN64_KEY_D:         return 0x0507;
    case CEN64_KEY_F:         return 0x0607;
    case CEN64_KEY_G:         return 0x0707;
    case CEN64_KEY_H:         return 0x0807;
    case CEN64_KEY_J:         return 0x0907;
    case CEN64_KEY_K:         return 0x0903;
    case CEN64_KEY_L:         return 0x0803;
    // case MISMATCH:         return 0x0703;
    // case MISMATCH:         return 0x0603;
    case CEN64_KEY_RETURN:    return 0x0D04;
    // 5th Row
    case CEN64_KEY_LSHIFT:    return 0x0E01;
    case CEN64_KEY_Z:         return 0x0D08;
    case CEN64_KEY_X:         return 0x0C08;
    case CEN64_KEY_C:         return 0x0508;
    case CEN64_KEY_V:         return 0x0608;
    case CEN64_KEY_B:         return 0x0708;
    case CEN64_KEY_N:         return 0x0808;
    case CEN64_KEY_M:         return 0x0908;
    case CEN64_KEY_COMMA:     return 0x0902;
    case CEN64_KEY_PERIOD:    return 0x0802;
    case CEN64_KEY_SLASH:     return 0x0702;
    case CEN64_KEY_UP:        return 0x0204;
    case CEN64_KEY_RSHIFT:    return 0x0E06;
    // 6th Row
    case CEN64_KEY_LCTRL:     return 0x1107;
    case CEN64_KEY_LMETA:     return 0x0F07;
    // case MISMATCH:         return 0x1105;
    case CEN64_KEY_LALT:      return 0x1008;
    // case MISMATCH:         return 0x1002;
    case CEN64_KEY_SPACE:     return 0x0602;
    // case MISMATCH:         return 0x0E02;
    // case MISMATCH:         return 0x1006;
    case CEN64_KEY_END:       return 0x0206;
    case CEN64_KEY_LEFT:      return 0x0205;
    case CEN64_KEY_DOWN:      return 0x0305;
    case CEN64_KEY_RIGHT:     return 0x0405;
    default:                  return 0x0000;
  }
}

void keyboard_press_callback(struct bus_controller *bus, unsigned key) {
  struct si_controller *si = bus->si;

  //fprintf(stderr, "os/input: Got keypress event: %u\n", key);

  if (si->keyboard.present) {
    if (key == CEN64_KEY_HOME) {
      si->keyboard.home_pressed = true;
      return;
    }
    uint16_t encoded = randnet_keyboard_key_encode(key);
    if (encoded) {
      uint16_t current;
      for (int i = 0; i < 4; ++i) {
        current = si->keyboard.keys_pressed[i];
        if (current == 0 || current == encoded) {
          si->keyboard.keys_pressed[i] = encoded;
          break;
        }
      }
    }
    return;
  }

  if (key == CEN64_KEY_LSHIFT || key == CEN64_KEY_RSHIFT) {
    shift_down = true;
    return;
  }

  switch (key) {
    // Analog stick.
    case CEN64_KEY_LEFT:
      si->input[2] = shift_down ? -38 : -114;
      left_down = true;
      break;

    case CEN64_KEY_RIGHT:
      si->input[2] = shift_down ? 38 : 114;
      right_down = true;
      break;

    case CEN64_KEY_UP:
      si->input[3] = shift_down ? 38 : 114;
      up_down = true;
      break;

    case CEN64_KEY_DOWN:
      si->input[3] = shift_down ? -38 : -114;
      down_down = true;
      break;

    // L/R flippers.
    case CEN64_KEY_A: si->input[1] |= 1 << 5; break;
    case CEN64_KEY_S: si->input[1] |= 1 << 4; break;

    // A/Z/B/S buttons.
    case CEN64_KEY_X: si->input[0] |= 1 << 7; break;
    case CEN64_KEY_C: si->input[0] |= 1 << 6; break;
    case CEN64_KEY_Z: si->input[0] |= 1 << 5; break;
    case CEN64_KEY_RETURN: si->input[0] |= 1 << 4; break;

    // D-pad.
    case CEN64_KEY_J: si->input[0] |= 1 << 1; break;
    case CEN64_KEY_L: si->input[0] |= 1 << 0; break;
    case CEN64_KEY_I: si->input[0] |= 1 << 3; break;
    case CEN64_KEY_K: si->input[0] |= 1 << 2; break;

    // C-pad.
    case CEN64_KEY_F: si->input[1] |= 1 << 1; break;
    case CEN64_KEY_H: si->input[1] |= 1 << 0; break;
    case CEN64_KEY_T: si->input[1] |= 1 << 3; break;
    case CEN64_KEY_G: si->input[1] |= 1 << 2; break;
  }
}

void keyboard_release_callback(struct bus_controller *bus, unsigned key) {
  struct si_controller *si = bus->si;

  //fprintf(stderr, "os/input: Got keyrelease event: %u\n", key);

  if (si->keyboard.present) {
    if (key == CEN64_KEY_HOME) {
      si->keyboard.home_pressed = false;
      return;
    }
    uint16_t encoded = randnet_keyboard_key_encode(key);
    if (encoded) {
      for (int i = 0; i < 4; ++i) {
        if (si->keyboard.keys_pressed[i] == encoded) {
          si->keyboard.keys_pressed[i] = 0x0000;
          break;
        }
      }
    }
    return;
  }

  if (key == CEN64_KEY_LSHIFT || key == CEN64_KEY_RSHIFT) {
    shift_down = false;
    return;
  }

  switch (key) {
    // Analog stick.
    case CEN64_KEY_LEFT:
      si->input[2] = right_down ? (shift_down ? 38 : 114) : 0;
      left_down = false;
      break;

    case CEN64_KEY_RIGHT:
      si->input[2] = left_down ? (shift_down ? -38 : -114) : 0;
      right_down = false;
      break;

    case CEN64_KEY_UP:
      si->input[3] = down_down ? (shift_down ? -38 : -114) : 0;
      up_down = false;
      break;

    case CEN64_KEY_DOWN:
      si->input[3] = up_down ? (shift_down ? 38 : 114) : 0;
      down_down = false;
      break;

    // L/R flippers.
    case CEN64_KEY_A: si->input[1] &= ~(1 << 5); break;
    case CEN64_KEY_S: si->input[1] &= ~(1 << 4); break;

    // A/Z/B/S buttons.
    case CEN64_KEY_X: si->input[0] &= ~(1 << 7); break;
    case CEN64_KEY_C: si->input[0] &= ~(1 << 6); break;
    case CEN64_KEY_Z: si->input[0] &= ~(1 << 5); break;
    case CEN64_KEY_RETURN: si->input[0] &= ~(1 << 4); break;

    // D-pad.
    case CEN64_KEY_J: si->input[0] &= ~(1 << 1); break;
    case CEN64_KEY_L: si->input[0] &= ~(1 << 0); break;
    case CEN64_KEY_I: si->input[0] &= ~(1 << 3); break;
    case CEN64_KEY_K: si->input[0] &= ~(1 << 2); break;

    // C-pad.
    case CEN64_KEY_F: si->input[1] &= ~(1 << 1); break;
    case CEN64_KEY_H: si->input[1] &= ~(1 << 0); break;
    case CEN64_KEY_T: si->input[1] &= ~(1 << 3); break;
    case CEN64_KEY_G: si->input[1] &= ~(1 << 2); break;
  }
}

