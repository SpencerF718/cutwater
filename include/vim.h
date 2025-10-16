#ifndef VIM_H
#define VIM_H

#include "cutwater.h"

// Holds Vim mode
typedef enum {
  NORMAL_MODE,
  INSERT_MODE,
} VimMode;

// Holds state information
typedef struct {
  CutwaterApp *app,
  VimMode mode;
} VimState;

// Initializes Vim motion implementation and integrates with text view
void vim_init(CutwaterApp *app);

#endif
