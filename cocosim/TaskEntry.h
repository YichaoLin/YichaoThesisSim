#pragma once
class TaskEntry
{
public:
	TaskEntry();
	~TaskEntry();

	int m_nID;
	CString m_strName;
	bool m_bFinished;
	CString m_strFinishComment;
};

