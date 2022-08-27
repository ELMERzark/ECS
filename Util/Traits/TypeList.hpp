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

	//���list�ϲ�Ϊһ��
	template<typename ... _List>
	struct TMergeMultiList;

	template<typename _List>
	struct TMergeMultiList<_List> {
		using type = _List;
	};

	template<typename _List0, typename ... _List1>
	struct TMergeMultiList<_List0, _List1...> {
		using type = TConcat_t<_List0, typename TMergeMultiList<_List1...>::type>;
	};




	///////////////////

		/********************��ȡ�䳤ģ������ĳ���******************/
	//Ч���ȼ���sizeof...(_Args)
	template<typename ... _Args>
	struct TGetLength;

	//�ػ�
	template<>
	struct TGetLength<> {
		static constexpr size_t value = 0;
	};

	template<typename _T, typename ..._Args>
	struct TGetLength<_T, _Args...> {
		static constexpr size_t value = TGetLength<_Args...>::value + 1;
	};

	template<typename ... _Args>
	constexpr size_t TGetLength_v = TGetLength<_Args...>::value;

	//list�汾
	template<typename _TypeList>
	struct TListLength;

	template<template<typename ...> typename _List, typename ... _Args>
	struct TListLength<_List<_Args...>> {
		static constexpr size_t value = TGetLength_v<_Args...>;
	};

	template<typename _TypeList>
	constexpr size_t TListLength_v = TListLength<_TypeList>::value;


	/*************************��ȡ�䳤ģ�������ĳ������_T������**********************/
	template<typename _T, typename ... _Args>
	struct TGetIndex;

	template<typename _T>
	struct TGetIndex<_T> {
		static constexpr size_t value = -1;
	};

	template<typename _T, typename _Arg, typename ... _Args>
	struct TGetIndex<_T, _Arg, _Args...> {
		static constexpr size_t value
			= std::is_same_v<_T, _Arg> ? 0 : (TGetIndex<_T, _Args...>::value + 1);
	};

	template<typename _T, typename ... _Args>
	constexpr size_t TGetIndex_v = TGetIndex<_T, _Args...>::value;

	//list�汾
	template<typename _TypeList, typename _T>
	struct TListGetIndex;

	template<template<typename ...> typename _List, typename _T, typename ... _Args>
	struct TListGetIndex<_List<_Args...>, _T> {
		static constexpr size_t value = TGetIndex_v<_T, _Args...>;
	};

	template<typename _TypeList, typename _T>
	constexpr size_t  TListGetIndex_v = TListGetIndex<_TypeList, _T>::value;

	/*******************************���ͱ�ǩ**************************/
	/*
	* �����Ǹ�ĳ����������һ����Ӧ�ı�ǩ����,ͬʱ����ȡ�����ɸñ�ǩ������
	*/
	template<typename _Type>
	struct TTag {
		using type = _Type;
	};

	/******************************����ӳ����************************/
	template<template<typename, typename > typename _TB, typename _Data>
	struct TTypeMap {
		template<typename _A>
		using type_tempalte = _TB<_A, _Data>;
	};

	/*******************************��������**************************/
	template<typename _Tag, typename _Data>
	struct TDerived : public _Data {
		using tag = _Tag;
	};

	/******************************���Ͱ�**********************************/
	/*
	* ��һ��2Ԫģ�����͵ĵڶ�������ʹ��Ϊ1Ԫģ������
	*/
	template<template<typename, typename> typename _ToBindType, typename _Data>
	struct TBind2nd {
		template<typename _Type>
		using template_type = _ToBindType<_Type, _Data>;
	};


	/********************************ȫ��**********************************/
	/*
	* ����Ϊ��������������������һ����С�����ȫ�����ȫ���������Ϊ�Ƚ����������
	* ������ģ�����
	*/
	template<typename ... _Type>
	struct TTotalOrder {
		template<typename _A, typename _B>
		struct TLess {
			static constexpr bool value = TGetIndex<_A, _Type...>::value < TGetIndex<_B, _Type...>::value;
		};
	};

	/********************************����********************************/
	/*
	* �ж�����_T�Ƿ񱻰��������Ͳ�������
	*/
	template<typename _T, typename ... _Args>
	struct Tcontain;
	template<typename _T, typename ... _Args>
	static constexpr bool Tcontain_v = Tcontain<_T, _Args...>::value;

	template<typename _T>
	struct Tcontain<_T> {
		static constexpr bool value = false;
	};

	template<typename _T, typename _Type, typename ... _Types>
	struct Tcontain<_T, _Type, _Types...> {
		static constexpr bool value = std::is_same_v<_T, _Type> ? true : Tcontain<_T, _Types...>::value;
	};


	/**********************************ȥ��***********************/
	/*
	* ����������ȥ��֮�󣬷���ʣ�����͵�TypeList
	*/
	template<template<typename...> typename TList, typename ... _Types> //forward declare
	struct TDuplicateRemoval;

	template<template<typename...> typename TList, typename _Type> //recursive terminate
	struct TDuplicateRemoval<TList, _Type> {
		using type = TList<_Type>;
	};

	template<template<typename...> typename TList, typename _Type, typename ... _Types>
	struct TDuplicateRemoval<TList, _Type, _Types...> {
		using type
			= std::conditional_t<TContain<TList<_Types...>, _Type>::value,
			typename TDuplicateRemoval<TList, _Types...>::type,
			TPushFront_t<typename TDuplicateRemoval<TList, _Types...>::type, _Type>>;
	};


	/*****************************�Ӷ������List����ȡ�����в��ظ����͵ķ���List*************/
	template<typename ... _TypeLists>
	struct TUnion {};
	template<typename ... _TypeLists>
	using TUnion_t = typename TUnion<_TypeLists...>::type;

	template<typename _TypeList>
	struct TUnion<_TypeList> {
		using type = typename TUnique<_TypeList>::type;
	};

	template<typename _TypeList1, typename _TypeList2>
	struct TUnion<_TypeList1, _TypeList2> {

	};

	template<template<typename...> typename TList, typename _TypeList1>
	struct TUnion<_TypeList1, TList<>> {
		using type = typename TUnion<_TypeList1>::type;
	};

	template<template<typename...> typename TList, typename _TypeList1, typename _Type, typename ... _Types>
	struct TUnion<_TypeList1, TList<_Type, _Types...>> {
		using type = typename TUnion<TUnique_t<TPushBack_t<_TypeList1, _Type>>, TList<_Types...>>::type;
	};

	template<typename _TypeList, typename ... _TypeLists>
	struct TUnion<_TypeList, _TypeLists...> {
		using type = typename TUnion <typename TUnion <_TypeList > ::type, typename TUnion<_TypeLists...>::type > ::type;
	};

	/*****************************��TypeListתΪ��Ӧ��tuple*************/
	template<typename _TypeList>
	struct TTypeListToTuple;

	template<typename _TypeList>
	using TTypeListToTuple_t = typename TTypeListToTuple<_TypeList>::type;

	template<typename ... _Args>
	struct TTypeListToTuple<TypeList<_Args...>> {
		using type = std::tuple<_Args...>;
	};

	/******************************���ϼ���*****************************/
	template<typename _TypeList1, typename _TypeList2>
	struct TSubAsSet;
	template<typename _TypeList1, typename _TypeList2>
	using TSubAsSet_t = typename TSubAsSet<_TypeList1, _TypeList2>::type;
	template<template<typename...> typename _List2, typename _TypeList1>
	struct TSubAsSet<_TypeList1, _List2<>> {
		using type = _TypeList1;
	};
	template<template<typename...> typename _List2, typename _TypeList1, typename _Head, typename ..._Tail>
	struct TSubAsSet<_TypeList1, _List2<_Head, _Tail...>> {
		using type = typename TSubAsSet<TErase_t<_TypeList1, _Head>, _List2<_Tail...>>::type;
	};

	/***********************************���죬�������죬�ƶ�����ֵ����������******************/
	template<typename _ComType>
	void construct(void* addr) {
		new (addr)_ComType();
	}
	template<typename _ComType>
	void copy_construct(void* dst, const void* src) {
		new (dst) _ComType(*static_cast<const _ComType*>(src));
	}
	template<typename _ComType>
	void move_construct(void* dst, void* src) {
		new(dst)_ComType(std::move(*static_cast<const _ComType*>(src)));
	}
	template<typename _ComType>
	void move_assign(void* dst, void* src) {
		*static_cast<_ComType*>(dst) = std::move(*static_cast<_ComType*>(src));
	}
	template<typename _ComType>
	void destruct(void* addr) {
		static_cast<_ComType*>(addr)->~_ComType();
	}

	/******************************����tuple������***************/
	template<typename ...Args>
	struct Tuple;

	template<typename _Arg>
	struct Tuple<_Arg> {
		_Arg arg_;
	};

	template<typename _Arg, typename ... _Args>
	struct Tuple<_Arg, _Args...> : public Tuple<_Args...> {
		_Arg arg_;
	};

	//��̬ע��,��_T����ע�ᵽ_DynamicTuple�еĵ�������
	template<typename _T, typename _Tuple>
	struct static_regist {
	};
	template<typename _T, typename ..._Args>
	struct static_regist<_T, Tuple<_Args...>> {
		using type = Tuple<_T, _Args...>;
	};

	//����ĳ��������ģ��ķ��������ģ��
	template<template<typename> typename Filter, typename _T>
	struct _ReverseFilter {
		static constexpr bool value = !Filter<_T>::value;
	};
	template<template<typename> typename Filter>
	struct TFilterReverse {
		template<typename _T>
		using type_template = _ReverseFilter <Filter, _T>;
	};

	//����ģ��List����һ����������λ��List�е�����ͨ�����ˣ���֮��ͨ��
	template<typename List, typename _T>
	struct _ListFilter {
		static constexpr bool value = TContain_v<List, _T>;
	};
	template<typename List>
	struct TFilterGenerator {
		template<typename _T>
		using type_template = _ListFilter<List, _T>;
	};


	template<typename _T>
	struct TAllPassFilter {
		static constexpr bool value = true;
	};
	template<typename _T>
	using TDefaultFilter = TAllPassFilter<_T>;

	//ʵ��tuple��ѭ������
	template <template<typename> typename Filter, typename Functor, typename _Ele>
	constexpr void for_each_tuple_impl2(Functor&& f, _Ele& t)
	{
		if constexpr (Filter<_Ele>::value == true) {
			f.operator() < _Ele > (t);
		}
	}
	template <template<typename> typename Filter, typename Tuple, typename Functor, std::size_t... Is>
	constexpr void for_each_tuple_impl(Tuple& t, Functor&& f, std::index_sequence<Is...>)
	{
		(for_each_tuple_impl2<Filter, Functor>(std::forward<Functor>(f), t.get<Is>()), ...);
	}
	template <template<typename> typename Filter, typename Tuple, typename Functor>
	constexpr void for_each_tuple(Tuple& t, Functor&& f)
	{
		for_each_tuple_impl<Filter, Tuple, Functor>(t, std::forward<Functor>(f), std::make_index_sequence<TListLength_v<Tuple>>());
	}

	template <typename Tuple, typename Functor>
	constexpr void for_each_tuple(Tuple& t, Functor&& f)
	{
		for_each_tuple<TDefaultFilter, Tuple, Functor>(t, std::forward<Functor>(f));
	}

	//��ȡ������ֽڶ�����Ϣ
	template<typename _ComType>
	struct TAlignSiz {
		static constexpr size_t value = sizeof(_ComType);
	};
	template<typename _ComType>
	static constexpr size_t TAlignSiz_v = TAlignSiz<_ComType>::value;

	/*******************��һ��TypeList��Ԫ�ؽ��б�������**********************/
	template <template<typename> typename Filter, typename Functor, typename Ele>
	constexpr void for_each_typelist_impl2(Functor&& f)
	{
		if constexpr (Filter<Ele>::value == true) {
			f.operator() < Ele > ();
		}
	}

	template <template<typename> typename Filter, typename List, typename Functor, std::size_t... Is>
	constexpr void for_each_typelist_impl(Functor&& f, std::index_sequence<Is...>)
	{
		(for_each_typelist_impl2<Filter, Functor, TAt_t<List, Is>>(std::forward<Functor>(f)), ...);
	}

	template <template<typename> typename Filter, typename List, typename Functor>
	constexpr void for_each_typelist(Functor&& f)
	{
		for_each_typelist_impl<Filter, List, Functor>(std::forward<Functor>(f), std::make_index_sequence<TLength_v<List>>());
	}

	template <typename List, typename Functor>
	constexpr void for_each_typelist(Functor&& f)
	{
		for_each_typelist<TDefaultFilter, List, Functor>(std::forward<Functor>(f));
	}

	//rebind
	template<typename A, template<typename...> typename B>
	struct rebind_;

	template<template<typename...> class A, class... T, template<typename...> typename B>
	struct rebind_<A<T...>, B> {
		using type = B<T...>;
	};

	template<typename A, template<typename...> typename B>
	using rebind = typename rebind_<A, B>::type;
}

#include "TypeList.inl"
