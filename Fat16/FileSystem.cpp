//
// Created by ASPCartman on 18/03/15.
// Copyright (c) 2015 Nistrom. All rights reserved.
//


#include <stdlib.h>
#include "FileSystem.h"
#include "ParseFunctions.h"

using namespace fat16;

FileSystem::FileSystem(byte *data)
{
	bootRecord          = new BootRecord(data);
	fileAllocationTable = new FileAllocationTable(data + 0x200, bootRecord->bytesPerBlock * bootRecord->blocksPerFAT);
	buildFileSystemTree(data);
};

void FileSystem::buildFileSystemTree(byte *data)
{
	byte *rootDirectoryInData = data + (bootRecord->fatsCount * bootRecord->blocksPerFAT + 1) * 512 * sizeof(byte);

	FileEntry *entry = new FileEntry();
	entry->name              = NULL;
	entry->directoryContents = new std::vector<FileEntry *>;
	parseFileEntries(entry->directoryContents, rootDirectoryInData, 1u, entry);
	rootDirectory = entry;
}

void FileSystem::parseFileEntries(std::vector<FileEntry *> *array, byte *data, uint currentClasterNumber, FileEntry *parent)
{
	byte *cursor      = data;
	byte *entryCursor = cursor;
	for (; ;)
	{
		byte attributeByte = *(cursor + 0x0b); // ЛУННАЯ ПРИЗМА, ДАЙ МНЕ СИЛ!!!
		if (attributeByte == 0x00)
		{
			break;
		}
		else if (attributeByte != 0x0f)
		{
			if (*(cursor) == 0x2e && (*(cursor + 1) == 0x2E || *(cursor + 1) == 0x20) && *(cursor + 2) == 0x20)
			{
				cursor += 32;
				entryCursor = cursor;
				continue;
			}
			FileEntry *entry = new FileEntry(entryCursor);
			entry->parentDirectory = parent;
			uint targetClasterNumber = 0;
			parseData(cursor, 0x1a, 2, (byte *) &targetClasterNumber);

			byte *targetClaster = getClasterPointer(data, currentClasterNumber, targetClasterNumber);

			if (attributeByte == 0x10)
			{
				parseFileEntries(entry->directoryContents, targetClaster, targetClasterNumber, entry);
			}
			else if (attributeByte == 0x20)
			{
				std::vector<FileEntry::FileData *> *content = new std::vector<FileEntry::FileData *>();

				byte *fileCluster      = targetClaster;
				uint fileClusterNumber = targetClasterNumber;
				uint fileSize          = entry->fileSize;
				for (; ;)
				{
					uint dataSize = std::min(fileSize, (bootRecord->blocksPerAllocationUnit * bootRecord->bytesPerBlock));
					fileSize -= dataSize;
					content->insert(content->end(), new FileEntry::FileData(fileCluster, dataSize));
					if (fileSize == 0)
					{
						break;
					}

					uint16_t nextFileClusterNumber = fileAllocationTable->at(fileClusterNumber);
					if (nextFileClusterNumber == NoNextCluster)
					{
						break;
					}

					fileCluster = getClasterPointer(fileCluster, fileClusterNumber, nextFileClusterNumber);
					fileClusterNumber = nextFileClusterNumber;
				}
				entry->fileData = content;
			}

			array->push_back(entry);

			cursor += 32;
			entryCursor         = cursor;
		}
		else
		{
			cursor += 32;
		}
	}
}

byte *FileSystem::getClasterPointer(byte *data, uint currentClasterNumber, uint targetClasterNumber)
{
	byte *targetClaster = data;
	if (currentClasterNumber == 1)
	{
		targetClaster += bootRecord->rootDirectoryLength * 32 + (targetClasterNumber - currentClasterNumber - 1) * (bootRecord->blocksPerAllocationUnit * bootRecord->bytesPerBlock);
	}
	else
	{
		targetClaster += (targetClasterNumber - currentClasterNumber) * (bootRecord->blocksPerAllocationUnit * bootRecord->bytesPerBlock);
	}
	return targetClaster;
}

FileSystem::~FileSystem()
{
	free(bootRecord);
	//TODO: Free directory structure
}

