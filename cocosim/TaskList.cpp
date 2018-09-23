#include "stdafx.h"
#include "TaskList.h"


TaskList::TaskList()
	: m_nFinishedCount(0)
{
}


TaskList::~TaskList()
{
}

void TaskList::RegisterEntry(const TaskEntry & newEntry)
{
	if (m_entries.find(newEntry.m_nID) != m_entries.cend())
	{
		return;
	}
	m_entries[newEntry.m_nID] = newEntry;
}

void TaskList::SetFinished(int nID, const CString & strComment)
{
	map<int, TaskEntry>::iterator ptr = m_entries.find(nID);
	if (ptr == m_entries.cend())
	{
		return;
	}
	if (ptr->second.m_bFinished == true)
	{
		return;
	}
	ptr->second.m_bFinished = true;
	ptr->second.m_strFinishComment = strComment;
	++m_nFinishedCount;
}
