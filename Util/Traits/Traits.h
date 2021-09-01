/*****************************************************************************
* @brief : Traits
* �ṩһЩ����ģ�庯��������ģ�����
* @author : acedtang
* @date : 2021/8/25
* @version : ver 1.0
* @inparam :
* @outparam :
*****************************************************************************/

#ifndef __SUN_TRAITS_H
#define __SUN_TRAITS_H

#include "TypeList.hpp"

namespace Sun {

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
	struct TGetLength<_T,_Args...> {
		static constexpr size_t value = TGetLength<_Args...>::value + 1;
	};

	template<typename ... _Args>
	constexpr size_t TGetLength_v = TGetLength<_Args...>::value;

	//list�汾
	template<typename _TypeList,template<typename ...> typename _List>
	struct TListGetLength;
	template<typename _TypeList, template<typename ...> typename _List>
	constexpr size_t TListGetLength_v = TListGetLength<_TypeList, _List>::value;

	template<template<typename ...> typename _List, typename ... _Args>
	struct TListGetLength<_List<_Args...>, _List> {
		static constexpr size_t value = TGetLength_v<_Args...>;
	};


	/*************************��ȡ�䳤ģ�������ĳ������_T������**********************/
	template<typename _T , typename ... _Args>
	struct TGetIndex;

	template<typename _T>
	struct TGetIndex<_T> {
		static constexpr size_t value = -1;
	};

	template<typename _T,typename _Arg, typename ... _Args>
	struct TGetIndex<_T, _Arg,_Args...> {
		static constexpr size_t value 
			= std::is_same_v<_T, _Arg> ? 0 : (TGetIndex<_T, _Args...>::value + 1);
	};

	template<typename _T, typename ... _Args>
	constexpr size_t TGetIndex_v = TGetIndex<_T, _Args...>::value;

	//list�汾
	template<typename _TypeList, typename _T, template<typename ...> typename _List>
	struct TListGetIndex;
	template<typename _TypeList, typename _T, template<typename ...> typename _List>
	constexpr size_t  TListGetIndex_v = TListGetIndex<_TypeList,_T,_List>::value;

	template<template<typename ...> typename _List, typename _T, typename ... _Args>
	struct TListGetIndex<_List<_Args...>, _T, _List> {
		static constexpr size_t value = TGetIndex_v<_T, _Args...>;
	};

	/*******************************���ͱ�ǩ**************************/
	/*
	* �����Ǹ�ĳ����������һ����Ӧ�ı�ǩ����,ͬʱ����ȡ�����ɸñ�ǩ������
	*/
	template<typename _Type>
	struct TTag {
		using type = _Type;
	};

	/******************************����ӳ����************************/
	template<template<typename,typename > typename _TB,typename _Data>
	struct TTypeMap {
		template<typename _A>
		using type_tempalte = _TB<_A,_Data>;
	};

	/*******************************��������**************************/
	template<typename _Tag,typename _Data>
	struct TDerived : public _Data{
		using tag = _Tag;
	};

	/******************************���Ͱ�**********************************/
	/*
	* ��һ��2Ԫģ�����͵ĵڶ�������ʹ��Ϊ1Ԫģ������
	*/
	template<template<typename,typename> typename _ToBindType, typename _Data>
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
		template<typename _A ,typename _B>
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
	struct Tcontain<_T,_Type,_Types...> {
		static constexpr bool value = std::is_same_v<_T, _Type> ? true : Tcontain<_T, _Types...>::value;
	};


	/**********************************ȥ��***********************/
	/*
	* ����������ȥ��֮�󣬷���ʣ�����͵�TypeList
	*/
	template<template<typename...> typename TList, typename ... _Types> //forward declare
	struct TDuplicateRemoval;

	template<template<typename...> typename TList, typename _Type> //recursive terminate
	struct TDuplicateRemoval<TList,_Type> {
		using type = TList<_Type>;
	};

	template<template<typename...> typename TList, typename _Type, typename ... _Types>
	struct TDuplicateRemoval<TList,_Type,_Types...> {
		using type
			= std::conditional_t<TContain<TList<_Types...>,_Type>::value,
			typename TDuplicateRemoval<TList, _Types...>::type,
			TPushFront_t<typename TDuplicateRemoval<TList, _Types...>::type, _Type>>;
	};
	
		
	/*****************************�Ӷ������List����ȡ�����в��ظ����͵ķ���List*************/
	template<typename ... _TypeLists>
	struct TUnion{};
	template<typename ... _TypeLists>
	using TUnion_t = typename TUnion<_TypeLists...>::type;

	template<typename _TypeList>
	struct TUnion<_TypeList>{
		using type = typename TUnique<_TypeList>::type;
	};

	template<typename _TypeList1,typename _TypeList2>
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
	template<typename _TypeList1,typename _TypeList2>
	struct TSubAsSet;
	template<typename _TypeList1, typename _TypeList2>
	using TSubAsSet_t = typename TSubAsSet<_TypeList1, _TypeList2>::type;
	template<template<typename...> typename _List2, typename _TypeList1>
	struct TSubAsSet<_TypeList1, _List2<>> {
		using type = _TypeList1;
	};
	template<template<typename...> typename _List2, typename _TypeList1, typename _Head,typename ..._Tail>
	struct TSubAsSet<_TypeList1, _List2<_Head, _Tail...>> {
		using type = typename TSubAsSet<TErase_t<_TypeList1, _Head>, _List2<_Tail...>>::type;
	};

	/***********************************���죬�������죬�ƶ�����ֵ����������******************/
	template<typename _ComType>
	void construct(void* addr) {
		new (addr)_ComType();
	}
	template<typename _ComType>
	void copy_construct(void* dst,const void* src) {
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

	template<typename _Arg,typename ... _Args>
	struct Tuple<_Arg,_Args...> : public Tuple<_Args...>{
		_Arg arg_;
	};

	//��̬ע��,��_T����ע�ᵽ_DynamicTuple�еĵ�������
	template<typename _T, typename _Tuple>
	struct static_regist {
	};
	template<typename _T ,typename ..._Args>
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
	template <template<typename> typename Filter, typename Functor ,typename _Ele>
	constexpr void for_each_tuple_impl2(Functor&& f, _Ele& t)
	{
		if constexpr (Filter<_Ele>::value == true) {
			f(t);
		}
	}
	template <template<typename> typename Filter, typename Functor, typename Tuple, std::size_t... Is>
	constexpr void for_each_tuple_impl(Functor&& f, Tuple& t, std::index_sequence<Is...>)
	{
		(for_each_tuple_impl2<Filter,Functor>(std::forward<Functor>(f),std::get<Is>(t)), ...);
	}
	template <template<typename> typename Filter, typename Functor,typename Tuple>
	constexpr void for_each_tuple(Functor&& f,Tuple& t)
	{
		for_each_tuple_impl<Filter, Functor,Tuple>(std::forward<Functor>(f), t, std::make_index_sequence<std::tuple_size_v<Tuple>>());
	}
	template <typename Functor, typename Tuple>
	constexpr void for_each_tuple(Functor&& f, Tuple& t)
	{
		for_each_tuple<TDefaultFilter, Functor, Tuple>(std::forward<Functor>(f), t);
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
			f.operator()<Ele>();
		}
	}

	template <template<typename> typename Filter, typename List, typename Functor, std::size_t... Is>
	constexpr void for_each_typelist_impl(Functor&& f, std::index_sequence<Is...>)
	{
		(for_each_typelist_impl2<Filter, Functor, TAt_t<List,Is>>(std::forward<Functor>(f)), ...);
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
}


#endif