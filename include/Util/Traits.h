#pragma once

#include <iterator>
#include <type_traits>
#include <utility>
#include <cassert>

// -----------------------------------------------------------------------------
// Forward Member Function
// -----------------------------------------------------------------------------

#define FORWARDING_MEMBER_FUNCTION(Inner, inner, function, qualifiers)                                               \
	template<typename... Args,                                                                                       \
			 typename return_type = decltype(std::declval<Inner qualifiers>().function(std::declval<Args &&>()...))> \
	constexpr decltype(auto) function(Args &&...args) qualifiers noexcept(                                           \
		noexcept(std::declval<Inner qualifiers>().function(std::forward<Args>(args)...))                             \
		and (std::is_reference<return_type>::value or std::is_nothrow_move_constructible<return_type>::value))       \
	{                                                                                                                \
		return /*static_cast<Inner qualifiers>*/ (inner).function(std::forward<Args>(args)...);                      \
	}

#define FORWARDING_MEMBER_FUNCTIONS_C(Inner, inner, function, reference) \
	FORWARDING_MEMBER_FUNCTION(Inner, inner, function, reference)        \
	FORWARDING_MEMBER_FUNCTION(Inner, inner, function, const reference)

#define FORWARDING_MEMBER_FUNCTIONS_CV(Inner, inner, function, reference)  \
	FORWARDING_MEMBER_FUNCTION(Inner, inner, function, reference)          \
	FORWARDING_MEMBER_FUNCTION(Inner, inner, function, const reference)    \
	FORWARDING_MEMBER_FUNCTION(Inner, inner, function, volatile reference) \
	FORWARDING_MEMBER_FUNCTION(Inner, inner, function, const volatile reference)

#define FORWARDING_MEMBER_FUNCTIONS(Inner, inner, function)   \
	FORWARDING_MEMBER_FUNCTIONS_CV(Inner, inner, function, &) \
	FORWARDING_MEMBER_FUNCTIONS_CV(Inner, inner, function, &&)

namespace utl
{
	/**
	 * @brief Class used as a Tag in Template Metaprogramming
	 */
	struct Nonesuch
	{
		Nonesuch(Nonesuch &&)	   = delete;
		Nonesuch(const Nonesuch &) = delete;

		void operator=(const Nonesuch &) = delete;
		void operator=(Nonesuch &&) = delete;
	};

	/**
	 * @brief Is the type complete (has implementation)
	 */
	template<typename T>
	concept complete_type = requires(T t)
	{
		sizeof(t);
	};

	/**
	 * @brief Is a arithmetic type
	 */
	template<typename T>
	concept arithmetic = std::is_arithmetic_v<std::remove_reference_t<T>>;

	/**
	 * @brief Is it aligned with
	 */
	template<typename T, typename U>
	concept aligned_with = std::alignment_of_v<T> == std::alignment_of_v<U>;

	// template<typename T, typename... EqualTypes>
	// concept matches = std::disjunction_v<std::is_same<std::remove_cv_t<T>, EqualTypes>...>;

	/**
	 * @brief Is type same as a bunch of types
	 */
	template<typename T, typename ...O>
	concept same_as = std::disjunction_v<std::is_same<T, O>...>;

	/**
	 * @brief Check of iterator is one of the types
	 */
	template<typename T, typename... U>
	concept iter_matches = std::disjunction_v<std::is_same<typename std::iterator_traits<T>::value_type, U>...>;

	/**
	 * @brief Strip type of pointer, reference, const and volatile
	 */
	template<typename T>
	using strip_t = std::remove_cv_t<std::remove_pointer_t<std::decay_t<T>>>;

	// -----------------------------------------------------------------------------
	// CRTP
	// -----------------------------------------------------------------------------

	/**
	 * @brief Provides cast to derived in CRTP
	 *
	 * @tparam T Derived class
	 * @tparam crtpType Base class
	 */
	template<typename T, template<typename> class crtpType>
	struct crtp
	{
		auto underlying() noexcept -> T * { return static_cast<T *>(this); }
		auto underlying() const noexcept -> const T * { return static_cast<const T *>(this); }

	private:
		crtp() = default;
		friend crtpType<T>;
	};

	// -----------------------------------------------------------------------------
	// CRTP like Mixins
	// -----------------------------------------------------------------------------

	template<typename T, template<typename> class Ex1, template<typename> class... Ex>
	struct MixBuilder
	{
		using type = Ex1<typename MixBuilder<T, Ex...>::type>;
	};

	template<typename T, template<typename> class Ex>
	struct MixBuilder<T, Ex>
	{
		using type = Ex<T>;
	};

	// -----------------------------------------------------------------------------
	// Functor Mixin using Concepts
	// -----------------------------------------------------------------------------

	template<template<typename...> class _T, typename _U, typename... _Z>
	requires requires()
	{
		typename _T<_U, _Z...>::base;
	}
	auto seperate(const _T<_U, _Z...> &) -> std::pair<_T<Nonesuch, _Z...>, typename _T<_U, _Z...>::base>;

	template<typename _T>
	requires requires()
	{
		typename _T::base;
	}
	auto seperate(const _T &) -> std::pair<_T, typename _T::base>;

	template<typename _T>
	auto seperate(const _T &) -> std::pair<_T, Nonesuch>;

	/**
	 * @brief Combines Mixins from Functor using given Mixins
	 * Extensions (Class<typename, typename, typename>) must inherit the last template parameter. CurrSec is the whole
	 * type to be unpacked. If a CurrSec type contains a 'using base = base_type' it will follow it. If a CurrSec type
	 * doesn't contain a 'using base = base_type' it will stop following it.
	 * @tparam F Functor having the template parameters: Tag, Remaining, Base Functor
	 * @tparam Base Base of the Functors
	 * @tparam CurrSec Highest Mixin type to operate from
	 */
	template<template<typename, typename, typename> class F, typename Base, typename CurrSec>
	struct Filter
	{
		using s		 = decltype(seperate(std::declval<CurrSec>()));
		using Fil	 = typename Filter<F, Base, typename s::second_type>::type;
		using F_Full = F<typename s::first_type, CurrSec, Fil>;
		using type	 = typename std::conditional<complete_type<F_Full>, F_Full, Fil>::type;
	};

	template<template<typename, typename, typename> class F, typename Base>
	struct Filter<F, Base, Nonesuch>
	{
		using type = Base;
	};

	// --------------------------------- Used Functor -----------------------------------------

	/**
	 * @brief Default Functor for Mixins construction
	 * @tparam Object Type
	 */
	template<typename T>
	class Functor
	{
	public:
		constexpr Functor() = default;
		constexpr explicit Functor(T *o)
			: m_o(o)
		{
		}

		constexpr auto obj(T *o) noexcept { m_o = o; }
		constexpr auto obj() const noexcept
		{
			assert(m_o != nullptr && "Object isn't assigned.");
			return m_o;
		}

	private:
		T *m_o;
	};

	// -----------------------------------------------------------------------------
	// Unify C-Arrays and Containers
	// -----------------------------------------------------------------------------

	namespace detail
	{
		template<typename T, std::size_t n>
		constexpr auto _value_type_(const T[n]) -> T;

		template<typename T>
		requires requires(T)
		{
			typename T::value_type;
		}
		constexpr auto _value_type_(T) -> typename T::value_type;

		template<typename T, std::size_t n>
		constexpr auto _iterator_cat_(const T[n]) -> std::random_access_iterator_tag;

		template<typename T>
		requires requires(T)
		{
			typename T::iterator::iterator_category;
		}
		constexpr auto _iterator_cat_(T) -> typename T::iterator::iterator_category;
	} // namespace detail

	template<typename T>
	using array_value_t = decltype(detail::_iterator_cat_(std::declval<T>()));

	template<typename T>
	using array_iter_cat_t = decltype(detail::_iterator_cat_(std::declval<T>()));

} // namespace utl