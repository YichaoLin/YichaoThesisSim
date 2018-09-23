#include "stdafx.h"
#include "CastleAlgorithmCommon.h"


CastleAlgorithmCommon::CastleAlgorithmCommon()
{
}


CastleAlgorithmCommon::~CastleAlgorithmCommon()
{
}

void CastleAlgorithmCommon::random_card(int* nums, int len)
{
	int temp = 0;
	int index = 0, now = 0;

	for (index = 0; index < len - 1; index++)
	{
		now = rand() % (len - index) + index;

		temp = nums[now];
		nums[now] = nums[index];
		nums[index] = temp;
	}
}
