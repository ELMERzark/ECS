
#ifndef __ECS_SAMPLE_TEST1_H
#define __ECS_SAMPLE_TEST1_H

#include "string.h"
#include "assert.h"
#include <iostream>

#include "../EntityMgr.h"


using namespace std;
using namespace Sun;

struct AAA
{
	int a;
	int mass;
	float b;
	std::string c;
};

class Test1 {
public:
	Test1() {}
	virtual ~Test1() {}

public:
	void TestEcs();
};
#endif