#pragma once
#include "TaskEntry.h"
#include <map>
using namespace std;

class TaskList
{
public:
	TaskList();
	~TaskList();

	map<int, TaskEntry> m_entries;
	void RegisterEntry(const TaskEntry & newEntry);
	void SetFinished(int nID, const CString & strComment);
	int m_nFinishedCount;
};

