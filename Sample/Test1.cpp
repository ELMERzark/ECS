#include "Test1.h"

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

	std::function<void(AAA& aaa, float& f, char c)> func = [](AAA& aaa, float& f, char c) {
		aaa.mass += 50;
	};
	mgr.entity_for_each(func);
	mgr.runAllTasks();
	assert(mgr.get<AAA>(eb1).mass == 50);
	assert(mgr.get<AAA>(eb2).mass == 50);
	assert(mgr.get<AAA>(eb3).mass == 50);

	//移动entity(等价于修改Entity的类型)
	mgr.moveEntity<ArchetypeB>(ea2);
	assert(mgr.exist(ea2) == true);
	assert(mgr.get<AAA>(ea2).mass == 100);

	assert(mgr.getArchetypeEntityNum<ArchetypeA>() == 2);
	assert(mgr.getArchetypeEntityNum<ArchetypeB>() == 4);
	assert(mgr.getArchetypeEntityNum<ArchetypeC>() == 3);
	assert(mgr.getTotalEntityNum() == 9);

	std::cout << "All test passed!\n";
	return;
}
