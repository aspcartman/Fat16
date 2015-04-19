//
// Created by ASPCartman on 18/03/15.
// Copyright (c) 2015 Nistrom. All rights reserved.
//


#ifndef __FATStructs_H_
#define __FATStructs_H_

#include <sys/types.h>
#include <iosfwd>
#include <vector>
#include <string>
#include "Types.h"

namespace fat16
{
	typedef enum {DIR,FILE} EntryType;
	struct BootRecord
	{
		uint bytesPerBlock;
		uint rootDirectoryLength;
		uint fatsCount;
		uint blocksPerFAT;
		uint blocksPerAllocationUnit;

		BootRecord(byte *bootRecord);
	};

	struct FileEntry
	{
		std::string *name;
		EntryType type;

		/*File*/
		struct FileData {
			byte *data;
			size_t len;

			FileData(byte *data, size_t len) : data(data), len(len){};
		};
		std::vector<FileData*> *fileData;
		uint fileSize;

		/*Directory*/
		std::vector<FileEntry*> *directoryContents = NULL;
		FileEntry *parentDirectory = NULL;

		FileEntry();
		FileEntry(byte *file);
	};

	const uint16_t NoNextCluster = 0xffff;
	struct FileAllocationTable : std::vector<uint16_t>
	{
		FileAllocationTable(byte *data, uint length);
	};

}




#endif //__FATStructs_H_
