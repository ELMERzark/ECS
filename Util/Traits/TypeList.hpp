#pragma once

#include <cstddef>
#include <functional>

//TypeList���к����Ѿ���ȫ������ϲ�����ע��
namespace Sun {

	template<typename ...>
	struct TTypeList{};

	template<template<typename ... > typename _ToBindType, typename _Data>
	struct TBind1st {
		template<typename ... _Args>
		using type_template = _ToBindType<_Data, _Args...>;
	};

	//specail case process ,not general
	template<template<template<typename...> typename ,typename,typename> typename _ToBindType, template<typename>typename _Data>
	struct TBind1st3 {
		template<typename _A, typename _B>
		using template_type = _ToBindType<_Data,_A,_B>;
	};

	//���б�ʵ�������ģ���� �����ģ������
	template<typename _Class>
	struct TTemplateType {
		template<typename ..._T>
		using type_template = TTypeList<_T...>;
	};
	template<template<typename...> typename _Template, typename ... _T>
	struct TTemplateType<_Template<_T...>> {
		template<typename ... _T>
		using type_template = _Template<_T...>;
	};

	template<typename... Ts>
	struct TypeList {};

	//for test
	template<typename ... Ts>
	struct TypeList2 {};

	template<template<typename...> typename T, typename... Ts>
	struct is_instantiable;
	template<template<typename...> typename T, typename... Ts>
	constexpr bool is_instantiable_v = is_instantiable<T, Ts...>::value;

	//�ж�����ģ���Ƿ���ͬ����ģ��
	template<template<typename...> class TA, template<typename...> class TB>
	struct is_same_typename_template;
	template<template<typename...> class TA, template<typename...> class TB>
	constexpr bool is_same_typename_template_v = is_same_typename_template<TA, TB>::value;

	//is_instance_of�жϵ�һ������Instance �Ƿ� ��_T<typename ..._Args>����
	template<typename Instance, template<typename...> class T>
	struct is_instance_of;
	template<typename Instance, template<typename...> class T>
	constexpr bool is_instance_of_v = is_instance_of<Instance, T>::value;

	//��ģ��OtherListTemplateģ��ʵ������ תΪ TyList����
	template<template<typename...>class OtherListTemplate, typename OtherList>
	struct ToTypeList;
	template<template<typename...>class OtherListTemplate, typename OtherList>
	using ToTypeList_t = typename ToTypeList<OtherListTemplate, OtherList>::type;

	//��TypeListʵ������תΪOtherListTemplateʵ������
	template<typename List, template<typename...>class OtherListTemplate>
	struct ToOtherList;
	template<typename List, template<typename...>class OtherListTemplate>
	using ToOtherList_t = typename ToOtherList<List, OtherListTemplate>::type;

	//�ж�һ��ģ���Ƿ���TypeListģ��
	template<typename List> struct IsTypeList;
	template<typename List> constexpr bool IsTypeList_v = IsTypeList<List>::value;

	/****************revise start here!!!****************************/
	//��ģ�����һ��TypeList������������ 
	template< typename List> 
	struct TLength;
	template<typename List> 
	constexpr std::size_t TLength_v = TLength<List>::value;

	//����һ��TypeList�Ƿ�Ϊ��,��ǰʵ������Length_v
	template<typename List> 
	struct TIsEmpty;
	template<typename List> 
	constexpr bool TIsEmpty_v = TIsEmpty<List>::value;

	//����TypeList��һ������
	template<typename List> 
	struct TFront;
	template<typename List> 
	using TFront_t = typename TFront<List>::type;

	//����indexof��ȡ��TypeList�ĵ�N������
	template<typename List, std::size_t N> 
	struct TAt;
	template<typename List, std::size_t N> 
	using TAt_t = typename TAt<List, N>::type;

	//ȡ����������Ӧ�������γ�TypeList
	template<typename List, std::size_t... Indices> 
	struct TSelect;
	template<typename List, std::size_t... Indices>
	using TSelect_t = typename TSelect<List, Indices...>::type;

	constexpr std::size_t Find_fail = static_cast<std::size_t>(-1);

	//Find���������ҳ�TypeList��T���͵�index+1�γ��ֵ����������û�г��߷���Find_fail
	template<typename List, typename T ,template<typename,typename> typename Equal = std::is_same> 
	struct TFind;
	template<typename List, typename T, template<typename, typename> typename Equal = std::is_same>
	constexpr std::size_t TFind_v = TFind<List, T ,Equal>::value;

	//Find��ʵ���е㸴�ӣ�������չΪ�ҵ�_T���͵�index+1�γ��ֵ�����
	template<typename List, typename _T, size_t index = 0>
	struct TIndex;
	template<typename List, typename _T, size_t index = 0>
	constexpr std::size_t TIndex_v = TIndex<List, _T,index >::value;
	template<template<typename ...> typename TList,typename _T, size_t index>
	struct TIndex<TList<>, _T, index> {
		static constexpr size_t value = Find_fail;
	};
	template<template<typename ...> typename TList, typename _Head, typename ..._Tail, typename _T>
	struct TIndex<TList<_Head, _Tail...>, _T, 0> {
		static constexpr size_t value = std::is_same_v<_T,_Head>?
			0: TIndex<TList<_Tail...>, _T, 0>::value + 1;
	};
	template<template<typename ...> typename TList, typename _Head,typename ..._Tail,typename _T,size_t index>
	struct TIndex<TList<_Head,_Tail...>,_T,index> {
		static constexpr size_t value = 
			std::is_same_v<_T, _Head>?
			TIndex<TList<_Tail...>, _T, index - 1>::value + 1:
			TIndex<TList<_Tail...>, _T, index>::value + 1;
	};

	//�ҵ�TypeList������Op<Type>�ĵ�һ��Type���ֵ�����
	template<typename List, template<typename>class Op> 
	struct TFindIf;
	template<typename List, template<typename>class Op> 
	constexpr std::size_t TFindIf_v = TFindIf<List, Op>::value;

	//����Find,����TypeList�Ƿ����T����
	template<typename List, typename T ,template<typename,typename> typename Equal = std::is_same>
	struct TContain;
	template<typename List, typename T, template<typename, typename> typename Equal = std::is_same>
	constexpr bool TContain_v = TContain<List, T, Equal>::value;

	//�����Ƿ����...Ts�е���������
	template<typename List, typename... Ts> 
	struct TContainTs;
	template<typename List, typename... Ts> 
	constexpr bool TContainTs_v = TContainTs<List, Ts...>::value;

	//����TypeList0�Ƿ����TypeList1�е���������
	template<typename List0, typename List1>
	struct TContainList;
	template<typename List0, typename List1>
	constexpr bool TContainList_v = TContainList<List0, List1>::value;

	//��List�Ĳ���Ϊ...Args,����T<Args...>�Ƿ�Ϸ�
	template<typename List, template<typename...> class T>
	struct TCanInstantiate;
	template<typename List, template<typename...> class T>
	constexpr bool TCanInstantiate_v = TCanInstantiate<List, T>::value;

	//��List�Ĳ���Ϊ...Args, ��ӦT<Args...>����
	template<typename List, template<typename...> class T>
	struct TInstantiate;
	template<typename List, template<typename...> class T>
	using TInstantiate_t = typename TInstantiate<List, T>::type;

	//����T�ܷ�ʵ����List�е�ĳ������ ��Ϊ��T��Ҫ��������?)
	template<typename List, template<typename...>class T>
	struct TExistInstance;
	template<typename List, template<typename...>class T>
	constexpr bool TExistInstance_v = TExistInstance<List, T>::value;

	// get first template instantiable type
	//����List�п��Ա�Tʵ�����ĵ�һ������
	template<typename List, template<typename...>class T>
	struct TSearchInstance;
	template<typename List, template<typename...>class T>
	using TSearchInstance_t = typename TSearchInstance<List, T>::type;

	//��TypeList֮ǰpush_front��_T�������õ�TypeList����
	template<typename List, typename T> 
	struct TPushFront;
	template<typename List, typename T> 
	using TPushFront_t = typename TPushFront<List, T>::type;

	//��TypeList֮��push_back��_T�������õ�TypeList����
	template<typename List, typename T> 
	struct TPushBack;
	template<typename List, typename T> 
	using TPushBack_t = typename TPushBack<List, T>::type;

	//��ʾTypeList�ĵ�һ��Ԫ��popfront֮��ʣ�µ�TypeList����
	template<typename List> 
	struct TPopFront;
	template<typename List> 
	using TPopFront_t = typename TPopFront<List>::type;

	//Ӧ���ǰ���Ԫ���ƶ���ĩβ��TypeList
	template<typename List> 
	struct TRotate;
	template<typename List> 
	using TRotate_t = typename TRotate<List>::type;

	//��List����_T��ͬ������ɾ������ʣ��List
	template<typename List,typename _T>
	struct TErase;
	template<typename List,typename _T>
	using TErase_t = typename TErase<List,_T>::type;
	
	//ȥ��
	template<typename List> 
	struct TUnique;
	template<typename List> 
	using TUnique_t = typename TUnique<List>::type;
	 
	//���ģ��Ƚ����棬��Op�ǰ���������ӳ��Ϊһ�����͵�ģ������,��ģ�壬����ΪI,
	//Op������I��TypeList�ĵ�һ�����͵õ�һ������T������Op������T��List�ĵڶ���������ӳ��õ�T1,
	//�����ݹ���ȥֱ��ListΪ�գ�������ĵ������� ,Op�����þͺ���+���������е�����һ��
	template<typename List, template <typename I, typename X> class Op, typename I>
	struct TAccumulate;
	template<typename List, template <typename I, typename X> class Op, typename I>
	using TAccumulate_t = typename TAccumulate< List, Op, I>::type;

	//Filter������Accumulate,�������ǹ��˳�TypeList�� ����Op<_T>::value==true �����ͣ������Щ���͵�
	//TypeList,����ò������Ƿ����)
	template<typename List, template<typename>class Op> 
	struct TFilter;
	template<typename List, template<typename>class Op> 
	using TFilter_t = typename TFilter<List, Op>::type;

	//��TypeList����
	template<typename List> 
	struct TReverse;
	template<typename List> 
	using TReverse_t = typename TReverse<List>::type;

	//��TypeList0��TypeList1ƴ������
	template<typename List0, typename List1> 
	struct TConcat;
	template<typename List0, typename List1> 
	using TConcat_t = typename TConcat<List0, List1>::type;

	//��Op������TypeListÿ��Ԫ�صõ�һ���µ�TypeList
	template<typename List, template<typename> class Op> 
	struct TTransform;
	template<typename List, template<typename> class Op> 
	using TTransform_t = typename TTransform<List, Op>::type;

	//������ʵ�Ķ����ͽ��п�������
	template<typename List, template<typename X, typename Y>typename Less>
	struct TQuickSort;
	template<typename List, template<typename X, typename Y>typename Less>
	using TQuickSort_t = typename TQuickSort< List, Less>::type;
	
	//��ʾTypeList�е�ÿ�������Ƿ�Ψһ
	template<typename List ,template<typename ,typename> typename Equal = std::is_same>
	struct TIsUnique;
	template<typename List, template<typename, typename> typename Equal = std::is_same>
	constexpr bool TIsUnique_v = TIsUnique<List, Equal>::value;

	//�Ƚ�����unique��list���Ƿ������ͬ��Ԫ��
	template<typename List1,typename List2>
	struct TEqualAsSet {
		static_assert(TIsUnique_v<List1>&& TIsUnique_v<List2>);
		static constexpr bool value = TContainList_v<List1, List2> && TContainList_v<List2, List1>;
	};
	template<typename List1, typename List2>
	constexpr bool TEqualAsSet_v = TEqualAsSet<List1, List2>::value;

	//��һ��listת��Ϊ����һ��List
	template< template<typename ...> typename TList2, typename List1>
	struct ListToList;
	template<template<typename ...> typename TList2, typename List1>
	using ListToList_t = typename ListToList<TList2, List1>::type;

}

#include "TypeList.inl"
