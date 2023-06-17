#include "Test1.h"
#include <chrono>
#include <iostream>
#include "assert.h"

long long Test1::CaculateTime(int timeType, std::chrono::system_clock::time_point start)
{
	auto end = std::chrono::system_clock::now();
	long long start_value = 0;
	long long end_value = 0;
	switch (timeType)
	{
	case 1:
		// "��"
		start_value = std::chrono::duration_cast<std::chrono::seconds>(start.time_since_epoch()).count();
		end_value = std::chrono::duration_cast<std::chrono::seconds>(end.time_since_epoch()).count();
		break;
	case 2:
		// "����"
		start_value = std::chrono::duration_cast<std::chrono::milliseconds>(start.time_since_epoch()).count();
		end_value = std::chrono::duration_cast<std::chrono::milliseconds>(end.time_since_epoch()).count();
		break;
	case 3:
		// "΢��"
		start_value = std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch()).count();
		end_value = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()).count();
		break;
	case 4:
		// "����"
		start_value = std::chrono::duration_cast<std::chrono::nanoseconds>(start.time_since_epoch()).count();
		end_value = std::chrono::duration_cast<std::chrono::nanoseconds>(end.time_since_epoch()).count();
		break;
	default:
		break;
	}

	return end_value - start_value;
}
std::default_random_engine __GENERATOR;

int Test1::GenerateRandom(int p_min, int p_max)
{
	std::uniform_int_distribution<int> distribution(p_min, p_max);
	return distribution(__GENERATOR);
}
void Test1::TestEcs()
{
	using ArchetypeA = Archetype<int, float, AAA>;
	using ArchetypeB = Archetype<int, float, AAA, char>;
	using ArchetypeC = Archetype<float, AAA>;

	EntityMgr<ArchetypeA, ArchetypeB, ArchetypeC> mgr;
	static_assert(mgr.getArchetypeNum() == 3);
	static_assert(mgr.getComponentNum() == 4);

	Entity ea1 = mgr.createEntity< ArchetypeA>();
	mgr.get<AAA>(ea1).mass = 100;
	mgr.get<int>(ea1) = 20;
	mgr.get<float>(ea1) = 35.6f;
	std::cout << "ea1 int:" << mgr.get<int>(ea1) << std::endl;
	std::cout << "ea1 float:" << mgr.get<float>(ea1) << std::endl;
	Entity ea2 = mgr.createEntity< ArchetypeA>();
	mgr.get<AAA>(ea2).mass = 100;
	Entity ea3 = mgr.createEntity< ArchetypeA>();
	mgr.get<AAA>(ea3).mass = 100;

	Entity eb1 = mgr.createEntity< ArchetypeB>();
	mgr.get<AAA>(eb1).mass = 50;
	Entity eb2 = mgr.createEntity< ArchetypeB>();
	mgr.get<AAA>(eb2).mass = 50;
	Entity eb3 = mgr.createEntity< ArchetypeB>();
	mgr.get<AAA>(eb3).mass = 50;

	Entity ec1 = mgr.createEntity< ArchetypeC>();
	mgr.get<AAA>(ec1).mass = 20;
	Entity ec2 = mgr.createEntity< ArchetypeC>();
	mgr.get<AAA>(ec2).mass = 20;
	Entity ec3 = mgr.createEntity< ArchetypeC>();
	mgr.get<AAA>(ec3).mass = 20;

	ArchetypeIter<ArchetypeB> iter = mgr.begin<ArchetypeB>();
	ArchetypeIter<ArchetypeB> iter2 = mgr.end<ArchetypeB>();
	for (iter; iter != iter2; ++iter) {
		iter->get<AAA>().mass -= 50;
	}
	assert(mgr.get<AAA>(eb1).mass == 0);
	assert(mgr.get<AAA>(eb2).mass == 0);
	assert(mgr.get<AAA>(eb3).mass == 0);

	/*std::function<void(AAA& aaa, float& f, char c)> func = [](AAA& aaa, float& f, char c) {
		aaa.mass += 50;
	};
	mgr.entity_for_each(func);
	mgr.runAllTasks();
	assert(mgr.get<AAA>(eb1).mass == 50);
	assert(mgr.get<AAA>(eb2).mass == 50);
	assert(mgr.get<AAA>(eb3).mass == 50);
	*/

	//�ƶ�entity(�ȼ����޸�Entity������)
	mgr.moveEntity<ArchetypeB>(ea2);
	assert(mgr.exist(ea2) == true);
	assert(mgr.get<AAA>(ea2).mass == 100);

	assert(mgr.getArchetypeEntityNum<ArchetypeA>() == 2);
	assert(mgr.getArchetypeEntityNum<ArchetypeB>() == 4);
	assert(mgr.getArchetypeEntityNum<ArchetypeC>() == 3);
	assert(mgr.getTotalEntityNum() == 9);

	std::cout << "All test passed!\n";

	auto start = std::chrono::system_clock::now();
	std::vector<Entity> vEntity;
	for (size_t i = 0; i < 1000000; i++)
	{
		Entity entityElement = mgr.createEntity<ArchetypeB>();
		vEntity.push_back(entityElement); 
	}
	long long duration = CaculateTime(2, start);
	std::cout << "��� ����1:" << duration << std::endl;


	start = std::chrono::system_clock::now();
	for_each(vEntity.begin(), vEntity.end(), [&](Entity& pair)
		{
			mgr.get<AAA>(pair).a += 45;
		});
	duration = CaculateTime(2, start);
	std::cout << "��� ����2:" << duration << std::endl;


	start = std::chrono::system_clock::now();
	std::function<void(AAA& aaa, float& f, char c)> func = [](AAA& aaa, float& f, char c) {
		aaa.mass += 50;
	};
	mgr.entity_for_each(func);
	mgr.runAllTasks();
	duration = CaculateTime(2, start);
	std::cout << "��� ����3:" << duration << std::endl;

	start = std::chrono::system_clock::now();
	std::function<void(AAA& aaa, float& f, char c)> func2 = [](AAA& aaa, float& f, char c) {
		aaa.mass += 50;
		f += 10.0f;
	};
	mgr.entity_for_each(func2);
	mgr.runAllTasks();
	duration = CaculateTime(2, start);
	std::cout << "��� ����6:" << duration << std::endl;

	start = std::chrono::system_clock::now();
	std::vector<NormalClass> vNormalEntity;
	for (size_t i = 0; i < 1000000; i++)
	{
		NormalClass entityElement;
		entityElement.m_a = GenerateRandom(1,4);
		vNormalEntity.push_back(entityElement);
	}
	duration = CaculateTime(2, start);
	std::cout << "��� ����4:" << duration << std::endl;


	start = std::chrono::system_clock::now();
	for_each(vNormalEntity.begin(), vNormalEntity.end(), [&](auto& pair)
		{
			if (3 == pair.m_a)
			{
				pair.m_a = 30;
			}
			if (2 == pair.m_a)
			{
				pair.m_a = 20;
			}
		});
	duration = CaculateTime(2, start);
	std::cout << "��� ����5:" << duration << std::endl;

	return;
}
