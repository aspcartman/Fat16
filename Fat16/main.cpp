//
//  main.cpp
//  Fat16
//
//  Created by ASPCartman on 18/03/15.
//  Copyright (c) 2015 Nistrom. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <assert.h>
#include <sstream>
#include "FATStructs.h"
#include "FileSystem.h"

using namespace std;
using namespace fat16;

byte                *getInputData();
std::vector<string> getCommand();

void pwd(FileEntry *currentDirectory);
void cat(FileEntry *entry);
void cd(FileEntry **currentDirectory, FileEntry *entry);
void ls(FileEntry *path);

vector<string> parseCommand(string &commandString);
vector<string> parsePath(string &path);
FileEntry      *resolvePath(FileEntry *cursor, vector<string> path);


int main(int argc, const char *argv[])
{
	byte *data = getInputData();

	FileSystem *fileSystem       = new fat16::FileSystem(data);
	FileEntry  *currentDirectory = fileSystem->rootDirectory;
	for (; ;)
	{
		vector<string> commandVector = getCommand();
		string         command       = commandVector[0];
		FileEntry      *arg          = nullptr;
		if (commandVector.size() == 2)
		{
			arg = resolvePath(currentDirectory, parsePath(commandVector[1]));
		}
		if (command == "ls")
		{
			ls(arg ? : currentDirectory);
		}
		else if (command == "cd")
		{
			cd(&currentDirectory, arg);
		}
		else if (command == "pwd")
		{
			pwd(currentDirectory);
		}
		else if (command == "cat")
		{
			cat(arg);
		}
		else if (command == "exit")
		{
			break;
		}
	}
	free(data);
	return 0;
}

byte *getInputData()
{
	byte *data = nullptr;

	ifstream input("Fat16Image.bin", ios_base::in | ios_base::binary | ios_base::ate);
	assert(input.is_open());

	auto size = input.tellg();
	assert(size);

	data = (byte *) malloc((size_t) (size * sizeof(byte)));
	assert(data);

	input.seekg(0, ios::beg);
	input.read(data, size);
	std::cout << input.gcount() << std::endl;
	input.close();

	return data;
}

vector<string> getCommand()
{
	string commandString;
	getline(cin, commandString);

	return parseCommand(commandString);
}

vector<string> parseCommand(string &commandString)
{
	uint      spaceindex = 0;
	for (uint i          = 0; i < commandString.size(); ++i)
	{
		char c = commandString[i];
		if (c == ' ')
		{
			spaceindex = i;
			break;
		}
	}

	vector<string> result;
	if (spaceindex == 0)
	{
		result.push_back(commandString);
		return result;
	}
	else
	{
		result.push_back(commandString.substr(0, spaceindex));
		result.push_back(commandString.substr(spaceindex + 1, commandString.size() - spaceindex - 1));
		return result;
	}
}

vector<string> parsePath(string &path)
{

	string p (path);

	/*1) Delete all // if present */
	for (int i = 0; i < p.size()-1; ++i)
	{
		if (p[i]=='/' && p[i+1] == '/')
		{
			p.erase(p.begin()+i);
		}
	}

	/* 2) Add / to the end */
	if (p.size() > 1 && p[p.size()-1] != '/')
	{
		p.append("/");
	}


	vector<string> result;
	uint           prevIndex = 0;
	for (uint      i         = 0; i < p.size(); ++i)
	{
		char c = p[i];
		if (c == '/' && i == 0)
		{
			result.push_back("/");
			prevIndex = i + 1;
		}
		else if (c == '/')
		{
			if (i == prevIndex)
			{
				prevIndex += 1;
				continue;
			}

			result.push_back(p.substr(prevIndex, i - prevIndex));
			prevIndex = i + 1;
		}
	}

	return result;
}

FileEntry *resolvePath(FileEntry *cursor, vector<string> path)
{

	for (uint i = 0; i < path.size(); ++i)
	{
		if (cursor->type == fat16::FILE)
		{
			return NULL;
		}

		string element = path[i];
		if (i == 0 && element == "/")
		{
			while (cursor->parentDirectory)
			{
				cursor = cursor->parentDirectory;
			}
		}
		else if (element == ".")
		{
		}
		else if (element == "..")
		{
			if (cursor->parentDirectory)
			{
				cursor = cursor->parentDirectory;
			}
			else
			{
//				return NULL;
			}
		}
		else
		{
			int            success = 0;
			for (FileEntry *entry : *cursor->directoryContents)
			{
				if (*entry->name == element)
				{
					cursor  = entry;
					success = 1;
					break;
				}
			}
			if (success == 0)
			{
				return NULL;
			}
		}
	}

	return cursor;
}


void ls(FileEntry *path)
{
	for (FileEntry *entry : *path->directoryContents)
	{
		cout << ((entry->type == DIR) ? "DIR" : "FILE") << " " << *entry->name << " " << entry->fileSize << endl;
	}
}

void cd(FileEntry **currentDirectory, FileEntry *dir)
{
	if (dir == nullptr || dir->type == fat16::FILE)
	{
		cout << "There is no such dir" << endl;
		return;
	}
	*currentDirectory = dir;
}

void cat(FileEntry *file)
{
	if (file == nullptr || file->type == fat16::DIR)
	{
		cout << "There is no such file" << endl;
		return;
	}
	for (FileEntry::FileData *data : *file->fileData)
	{
		cout << string(data->data, data->len);
	}
	cout << endl;
}

void pwd(FileEntry *currentDirectory)
{
	string    result;
	FileEntry *cursor = currentDirectory;
	if (currentDirectory->parentDirectory == nullptr)
	{
		result.append("/");
	}
	else
	{
		do
		{
			if (cursor->name)
			{
				result.insert(0, *cursor->name);
			}
			if (cursor->parentDirectory)
			{
				result.insert(0, "/");
			}
		}
		while ((cursor = cursor->parentDirectory));
	}
	cout << result << endl;
}



