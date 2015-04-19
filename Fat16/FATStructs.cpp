//
// Created by ASPCartman on 18/03/15.
// Copyright (c) 2015 Nistrom. All rights reserved.
//

#include "FATStructs.h"
#include "ParseFunctions.h"

using namespace fat16;

BootRecord::BootRecord(byte *bootRecord)
{
	parseData(bootRecord, 0x0b, 2, (byte *) &bytesPerBlock);
	parseData(bootRecord, 0x10, 1, (byte *) &fatsCount);
	parseData(bootRecord, 0x11, 2, (byte *) &rootDirectoryLength);
	parseData(bootRecord, 0x16, 2, (byte *) &blocksPerFAT);
	parseData(bootRecord, 0x0d, 1, (byte *) &blocksPerAllocationUnit);
}

FileEntry::FileEntry(byte *data)
{
	std::vector<byte *> entriesData;

	byte *cursor = data;
	for (; ;)
	{
		byte attributeByte = *(cursor + 0x0b);
		if (attributeByte == 0x00)
		{
			break;
		}

		entriesData.insert(entriesData.begin(), cursor);
		cursor += 32;

		if (attributeByte != 0x0f)
		{
			break;
		}
	}

	for (uint i = 0; i < entriesData.size(); ++i)
	{
		byte *entry = entriesData[i];
		if (i == 0)
		{
			if (entriesData.size() == 1)
			{
				name = new std::string(entry + 0x00, 11);
			}

			byte attribute = 0;
			parseData(entry, 0x0b, 1, &attribute);
			type = attribute == 0x10 ? DIR : FILE;

			if (type == DIR)
			{
				directoryContents = new std::vector<FileEntry *>();
			}

			uint size = 0;
			parseData(entry, 0x1c, 4, (byte *) &size);
			fileSize = size;
		}
		else
		{
			if (i == 1)
			{
				name = new std::string();
			}

			char string[13] = { 0 };
			parseDataString(entry, 0x01, 10, string);
			parseDataString(entry, 0x0e, 12, string + 5);
			parseDataString(entry, 0x1c, 4, string + 5 + 6);

			name->append(string);
		}
	}
}

FileEntry::FileEntry()
{
}

FileAllocationTable::FileAllocationTable(byte *data, uint length)
{
	length = length / 2;
	for (uint i = 0; i < length; ++i)
	{
		uint16_t buffer = 0;
		parseData(data, i * 2, 2, (byte *) &buffer);
		insert(end(), buffer);
	}
}
