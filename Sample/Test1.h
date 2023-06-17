
#ifndef __ECS_SAMPLE_TEST1_H
#define __ECS_SAMPLE_TEST1_H

#include "string.h"

#include "../EntityMgr.h"


using namespace std;
using namespace Sun;

struct AAA
{
	int a;
	int mass;
	int hp;
	float b;
	std::string c;
	AAA():a(0),mass(0),hp(0),b(0.0f),c("")
	{}
};
class NormalClass
{
public:
	NormalClass() {}
	virtual ~NormalClass() {}
public:
	AAA m_struct;
	int m_a;
};

class Test1 {
public:
	Test1() {}
	virtual ~Test1() {}

public:
	void TestEcs();
	//计算消耗时间
	long long CaculateTime(int timeType, std::chrono::system_clock::time_point start);
};
#endif