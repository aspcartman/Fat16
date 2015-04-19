//
// Created by ASPCartman on 18/03/15.
// Copyright (c) 2015 Nistrom. All rights reserved.
//


#ifndef __FileSystem_H_
#define __FileSystem_H_

#include <vector>
#include "FATStructs.h"

namespace fat16
{
	class FileSystem
	{
	public:
		FileSystem(byte *data);
		~FileSystem();
		FileEntry *rootDirectory;

	protected:
		BootRecord *bootRecord;
		FileAllocationTable *fileAllocationTable;

		void buildFileSystemTree(byte *data);
		void parseFileEntries(std::vector<FileEntry*> *array, byte *data, uint currentClasterNumber, FileEntry *parent);
		byte *getClasterPointer(byte *data, uint currentClasterNumber, uint targetClasterNumber);
	};
}


#endif //__FileSystem_H_
