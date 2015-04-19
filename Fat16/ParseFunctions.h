//
// Created by ASPCartman on 18/03/15.
// Copyright (c) 2015 Nistrom. All rights reserved.
//


#ifndef __ParseFunctions_H_
#define __ParseFunctions_H_

#include <sys/types.h>
#include "Types.h"

int parseData(byte *data, uint offset, uint length, byte *output);
int parseDataString(byte *data, uint offset, uint length, char *output);


#endif //__ParseFunctions_H_
