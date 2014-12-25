//
// rsp/cp2.h: RSP control coprocessor.
//
// CEN64: Cycle-Accurate Nintendo 64 Simulator.
// Copyright (C) 2014, Tyler J. Stachecki.
//
// This file is subject to the terms and conditions defined in
// 'LICENSE', which is part of this source code package.
//

#ifndef __rsp_cp2_h__
#define __rsp_cp2_h__
#include "common.h"
#include "rsp/rsp.h"

enum rsp_acc_t {
  RSP_ACC_LO = 16,
  RSP_ACC_MD = 8,
  RSP_ACC_HI = 0,
};

union aligned_rsp_vect_t {
  rsp_vect_t __align;
  uint16_t e[8];
};

union aligned_rsp_acc_t {
  rsp_vect_t __align[3];
  uint16_t e[24];
};

struct rsp_cp2 {
  union aligned_rsp_vect_t regs[32];

  union aligned_rsp_vect_t vcc[2];
  union aligned_rsp_vect_t vco[2];
  union aligned_rsp_vect_t vce;

  union aligned_rsp_acc_t acc;

  int16_t div_out;
  int16_t div_in;
  char dp_flag;
};

void RSP_CFC2(struct rsp *rsp, uint32_t iw, uint32_t rs, uint32_t rt);
void RSP_MFC2(struct rsp *rsp, uint32_t iw, uint32_t rs, uint32_t rt);

#endif

