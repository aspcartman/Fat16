//
// Created by ASPCartman on 18/03/15.
// Copyright (c) 2015 Nistrom. All rights reserved.
//

#include <assert.h>
#include "ParseFunctions.h"

int parseData(byte *data, uint offset, uint length, byte *output)
{
	assert(data != nullptr && output != nullptr);

	int count = 0;
	for (uint cursor = offset; cursor < offset + length; ++cursor)
	{
		*(output + cursor - offset) = *(data + cursor);
		++count;
	}
	return count;
}

int parseDataString(byte *data, uint offset, uint length, char *output)
{
	assert(data != nullptr && output != nullptr);

	int count = 0;
	for (uint cursor = offset, i=0; cursor < offset + length; ++cursor, ++i)
	{
		if (i%2)
		{
			continue;
		}
		char c = *(data + cursor);
		if (c == 0)
		{
			break;
		}
		*(output + i/2) = c;
		++count;
	}
	return count;
}