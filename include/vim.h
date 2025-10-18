#ifndef VIM_H
#define VIM_H

//#include "cutwater.h"

typedef struct CutwaterApp CutwaterApp;
typedef struct VimState VimState;

// Holds Vim mode
typedef enum {
  NORMAL_MODE,
  INSERT_MODE,
  OPERATOR_PENDING_MODE,
} VimMode;

// Holds state information
struct VimState{
  CutwaterApp *app;
  VimMode mode;
  gint saved_col;
  char pending_operator;
};

// Initializes Vim motion implementation and integrates with text view
void vim_init(CutwaterApp *app);

#endif
