#ifndef FILE_IO_H
#define FILE_IO_H

#include "cutwater.h"

void save_file(CutwaterApp *app);
void open_file_action(GSimpleAction *action, GVariant *parameter, gpointer user_data);
void save_file_action(GSimpleAction *action, GVariant *parameter, gpointer user_data);

#endif

