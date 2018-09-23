#pragma once

typedef enum {
	CASTLE_ALG_PHASE_WRITE = 0,
	CASTLE_ALG_PHASE_STRATEGY,
	CASTLE_ALG_PHASE_ACTION,
	CASTLE_ALG_PHASE_READ,
	CASTLE_ALG_PHASE_CALCULATE,
	CASTLE_ALG_PHASE_MOVE,
}CASTLE_ALG_PHASE;

class CastleAlgorithmCommon
{
public:
	CastleAlgorithmCommon();
	~CastleAlgorithmCommon();

	static void random_card(int* nums, int len);
};

