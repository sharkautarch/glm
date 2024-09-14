/// @ref core
/// @file glm/detail/simd_constexpr/vec.hpp

#pragma once

#include "../qualifier.hpp"
#if GLM_CONFIG_SWIZZLE == GLM_SWIZZLE_OPERATOR
#	include "../_swizzle.hpp"
#elif GLM_CONFIG_SWIZZLE == GLM_SWIZZLE_FUNCTION
#	include "../_swizzle_func.hpp"
#endif
#include <cstddef>
#include <array>
#include <variant>
#include <cstring>
#include <ranges>
namespace glm
{
	template <length_t L>
	concept NotVec1 = !std::is_same_v<std::integral_constant<length_t, L>, std::integral_constant<length_t, 1>>;
	template <qualifier Q>
	consteval bool BIsAlignedQ() {
		return Q == aligned_highp || Q == aligned_mediump || Q == aligned_lowp;
	}
	
	template <qualifier Q, qualifier Qx>
	consteval bool BRequiresPackOrUnpack() {
		return BIsAlignedQ<Q> ^ BIsAlignedQ<Qx>;
	}
	template <typename T>
	concept arithmetic = std::integral<T> || std::floating_point<T>;
	template <typename T0, typename... T>
	consteval bool SameArithmeticTypes() {
		return (std::is_same_v<std::common_type_t<T0,T>, std::common_type_t<T0,T>> && ...);
	}
	template <typename... T>
	consteval bool NotSameArithmeticTypes() {
		return ( (!(std::is_integral_v<T> || std::is_floating_point_v<T>) || ...) || !(SameArithmeticTypes<T...>()) );
	}
	
	namespace detail
	{
		template <length_t L, typename T, qualifier Q>
		using _ArrT = std::array<T,L>;

		template <length_t L, typename T, qualifier Q>
		using _data_t = typename detail::storage<L, T, detail::is_aligned<Q>::value>::type;
		
		template <length_t L, typename T, qualifier Q>
		struct GccVExt {
			static constexpr length_t v_length = (L == 3) ? 4 : L;
			using VType = std::conditional_t< std::is_same_v<T, bool>, uint8_t, T>;
			typedef VType GccV __attribute__(( vector_size(sizeof(VType)*v_length), aligned(alignof(_data_t<L, T, Q>)) ));
		};
		template <length_t L, typename T, qualifier Q>
		consteval bool BDataNeedsPadding() {
			return sizeof(_data_t<L,T,Q>) > sizeof(_ArrT<L,T,Q>);
		}
		template <length_t L, typename T, qualifier Q>
		consteval bool BVecNeedsPadding() {
			return sizeof(_data_t<L,T,Q>) > sizeof(typename GccVExt<L,T,Q>::GccV);
		}
		template <length_t L, typename T, qualifier Q, bool NeedsPadding>
		struct VecDataArray;
			
		template <length_t L, typename T, qualifier Q>
		struct VecDataArray<L, T, Q, true> {
			using ArrT = _ArrT<L, T, Q>;
			using data_t = _data_t<L,T,Q>;
			ArrT p;
			std::byte padding[sizeof(data_t) - sizeof(ArrT)];
		};
		template <length_t L, typename T, qualifier Q>
		struct VecDataArray<L, T, Q, false> {
			using ArrT = _ArrT<L, T, Q>;
			ArrT p;
		};
		
		template <length_t L, typename T, qualifier Q, bool NeedsPadding>
		struct PaddedGccVec;
		
		template <length_t L, typename T, qualifier Q>
		struct PaddedGccVec<L, T, Q, true> {
			using GccV = typename GccVExt<L, T,Q>::GccV;
			using data_t = _data_t<L, T, Q>;
			GccV gcc_vec;
			std::byte padding[sizeof(data_t) - sizeof(GccV)];
		};
	
		template <length_t L, typename T, qualifier Q>
		struct PaddedGccVec<L, T, Q, false> {
			using GccV = typename GccVExt<L, T,Q>::GccV;
			GccV gcc_vec;
		};
	}
	
	template <length_t L, typename T, qualifier Q>
	using PaddedGccVec = detail::PaddedGccVec<L, T, Q, detail::BVecNeedsPadding<L, T, Q>()>;
	
	template <length_t L, typename T, qualifier Q>
	using VecDataArray = detail::VecDataArray<L, T, Q, detail::BDataNeedsPadding<L, T, Q>()>;
	
}
#include "element.hpp"
#include "simd_helpers.inl"
#include "../compute_vector_relational.hpp"
#include "../compute_vector_decl.hpp"
#if GLM_CONFIG_SIMD == GLM_ENABLE
#	include "../type_vec_simd.inl"
#endif
namespace glm
{
	
	template <length_t L, typename T, qualifier Q>
	struct DataWrapper { // stupid wrapper to silence a warning: https://stackoverflow.com/a/59993590
		using data_t = detail::_data_t<L, T, Q>;
	};
	template <length_t L, typename T, qualifier Q>
	using EC = detail::ElementCollection<Q, T, L>;
	template<length_t L, typename T, qualifier Q>
	struct vec : detail::ElementCollection<Q, T, L>
	{
		// -- Data --
		using EC<L, T, Q>::x;
		using EC<L, T, Q>::y;
		using EC<L, T, Q>::z;
		using EC<L, T, Q>::w;
		using EC<L, T, Q>::r;
		using EC<L, T, Q>::g;
		using EC<L, T, Q>::b;
		using EC<L, T, Q>::a;
		using EC<L, T, Q>::s;
		using EC<L, T, Q>::t;
		using EC<L, T, Q>::p;
		using EC<L, T, Q>::q;

		using SimdHlp = detail::SimdHelpers<L, T, Q>;
		using DataArray = VecDataArray<L, T, Q>;
		using data_t = typename detail::storage<L, T, detail::is_aligned<Q>::value>::type;

		// -- Implementation detail --
		typedef T value_type;
		typedef vec<L, T, Q> type;
		typedef vec<L, bool, Q> bool_type;
		static constexpr qualifier k_qual = Q;
		static constexpr length_t k_len = L;
		
		enum is_aligned
		{
			value = detail::is_aligned<Q>::value
		};

		// -- Component Access --
		static constexpr length_t length(){	return L;	}
		
		inline constexpr T& operator[](length_t i)
		{
			if (!std::is_constant_evaluated()) {
				GLM_ASSERT_LENGTH(i, L);
			}
			switch (i)
			{
				default:
				case 0:
					return x;
				case 1: {
					if constexpr (L>=2)
						return y;
					else
						__builtin_unreachable();
				}
				case 2:{
					if constexpr (L>=3)
						return z;
					else
						__builtin_unreachable();
				}
				case 3:{
					if constexpr (L>=4)
						return w;
					else
						__builtin_unreachable();
				}
			}
		}
		
		inline constexpr T operator[](length_t i) const
		{
			if (!std::is_constant_evaluated()) {
				GLM_ASSERT_LENGTH(i, L);
			}
			switch (i)
			{
				default:
				case 0:
					return x;
				case 1: {
					if constexpr (L>=2)
						return y;
					else
						__builtin_unreachable();
				}
				case 2:{
					if constexpr (L>=3)
						return z;
					else
						__builtin_unreachable();
				}
				case 3:{
					if constexpr (L>=4)
						return w;
					else
						__builtin_unreachable();
				}
			}
		}
		
		
#				if GLM_CONFIG_SWIZZLE == GLM_SWIZZLE_OPERATOR
					GLM_SWIZZLE4_2_MEMBERS(T, Q, x, y, z, w)
					GLM_SWIZZLE4_2_MEMBERS(T, Q, r, g, b, a)
					GLM_SWIZZLE4_2_MEMBERS(T, Q, s, t, p, q)
					GLM_SWIZZLE4_3_MEMBERS(T, Q, x, y, z, w)
					GLM_SWIZZLE4_3_MEMBERS(T, Q, r, g, b, a)
					GLM_SWIZZLE4_3_MEMBERS(T, Q, s, t, p, q)
					GLM_SWIZZLE4_4_MEMBERS(T, Q, x, y, z, w)
					GLM_SWIZZLE4_4_MEMBERS(T, Q, r, g, b, a)
					GLM_SWIZZLE4_4_MEMBERS(T, Q, s, t, p, q)
					
					GLM_SWIZZLE3_2_MEMBERS(T, Q, x, y, z)
					GLM_SWIZZLE3_2_MEMBERS(T, Q, r, g, b)
					GLM_SWIZZLE3_2_MEMBERS(T, Q, s, t, p)
					GLM_SWIZZLE3_3_MEMBERS(T, Q, x, y, z)
					GLM_SWIZZLE3_3_MEMBERS(T, Q, r, g, b)
					GLM_SWIZZLE3_3_MEMBERS(T, Q, s, t, p)
					GLM_SWIZZLE3_4_MEMBERS(T, Q, x, y, z)
					GLM_SWIZZLE3_4_MEMBERS(T, Q, r, g, b)
					GLM_SWIZZLE3_4_MEMBERS(T, Q, s, t, p)
					
					GLM_SWIZZLE2_2_MEMBERS(T, Q, x, y)
					GLM_SWIZZLE2_2_MEMBERS(T, Q, r, g)
					GLM_SWIZZLE2_2_MEMBERS(T, Q, s, t)
					GLM_SWIZZLE2_3_MEMBERS(T, Q, x, y)
					GLM_SWIZZLE2_3_MEMBERS(T, Q, r, g)
					GLM_SWIZZLE2_3_MEMBERS(T, Q, s, t)
					GLM_SWIZZLE2_4_MEMBERS(T, Q, x, y)
					GLM_SWIZZLE2_4_MEMBERS(T, Q, r, g)
					GLM_SWIZZLE2_4_MEMBERS(T, Q, s, t)
#				endif
		
		template <typename ScalarGetter>
		static constexpr auto ctor_scalar(ScalarGetter scalar) {
			if (std::is_constant_evaluated()) {
				DataArray a;
		    for (length_t i = 0; i < L; i++) {
		    	a.p[i]=scalar();
		    }
		    return std::bit_cast<data_t>(a);
			} else {
				return SimdHlp::simd_ctor_scalar(scalar());
			}
		}
		
		template <typename VecGetter>
		static constexpr auto ctor(VecGetter vecGetter) {
			if (std::is_constant_evaluated()) {
				DataArray a = {};
				auto v = vecGetter();
				constexpr length_t vL = v.length();
				using ArrX = VecDataArray<vL, typename decltype(v)::value_type, decltype(v)::k_qual>;
				ArrX ax = std::bit_cast<ArrX>(v.data);
				for (length_t i = 0; i < v.length(); i++) {
					a.p[i] = (T)ax.p[i];
				}
				
				return std::bit_cast<data_t>(a);
			} else {
				return SimdHlp::simd_ctor(vecGetter());
			}
		}
		template <length_t len>
		using RetArr = T[len];
		
		template <typename Vs0>
		static constexpr length_t ctor_mixed_constexpr_single_get_length()
		{
			if constexpr ( std::is_integral_v<Vs0> || std::is_floating_point_v<Vs0> ) {
				return 1;
			} else if constexpr ( ( requires { Vs0::k_len; }) ) {
				return Vs0::k_len;
			} else {
				return 1;
			} 
		}
		static inline auto ctor_mixed_constexpr_single = [](auto vs0) -> auto
		{
			using VTX = decltype(vs0);
			if constexpr ( std::is_integral_v<VTX> || std::is_floating_point_v<VTX> ) {
				return RetArr<1>{vs0};
			} else if constexpr ( ( requires { VTX::k_len; }) ) {
				using Tx = VTX::value_type;
				using ArrX = VecDataArray<VTX::k_len, Tx, VTX::k_qual>;
				
				ArrX ax = std::bit_cast<ArrX>(vs0.data);
				return ax;
			} else {
				using Tx = VTX::value_type;
				return RetArr<1>{(Tx)vs0};
			} 
		};
		
		constexpr vec() = default;
		constexpr vec(arithmetic auto scalar) : EC<L, T, Q>{.data= [scalar](){ auto s = [scalar](){ return scalar; }; return ctor_scalar(s); }() } {}

		template <length_t Lx, typename Tx, qualifier Qx> requires (Lx == 1 && NotVec1<L>)
	  constexpr vec(vec<Lx, Tx, Qx> v) : EC<L, T, Q>{.data= [d=std::bit_cast<VecDataArray<Lx, Tx, Qx>>(v.data)](){ auto s = [scalar=d.p[0]](){ return scalar; }; return ctor_scalar(s); }() } {}
		
		template <length_t Lx, typename Tx, qualifier Qx> requires (L == 1 || Lx != 1)
		constexpr vec(vec<Lx, Tx, Qx> v) : EC<L, T, Q>{.data= [v](){ auto vv = [v](){ return v; }; return ctor(vv); }() } {}
		
		template <arithmetic... Scalar> requires (sizeof...(Scalar) == L)
		constexpr vec(Scalar... scalar)
		: EC<L, T, Q>
			{.data= [scalar...]() -> data_t
				{
					if (std::is_constant_evaluated()) {
						DataArray a = {.p={ T(scalar)... }};
						return std::bit_cast<data_t>(a);
					} else {
						return SimdHlp::simd_ctor_multi_scalars(scalar...);
					}
				}()
			} {}
			
		template <typename VecOrScalar0, typename... VecOrScalar> requires (sizeof...(VecOrScalar) >= 1 && NotSameArithmeticTypes<VecOrScalar0, VecOrScalar...>())
		constexpr vec(VecOrScalar0 vecOrScalar0, VecOrScalar... vecOrScalar)
		: EC<L, T, Q>
			{.data= [vecOrScalar0, vecOrScalar...]() -> data_t
				{
					//type_vecx.inl never had any simd versions for ctor from mixes of scalars & vectors,
					//so I don't really need to figure out how I'd make a generic simd version for this ctor 
					
					constexpr auto i = ctor_mixed_constexpr_single_get_length<VecOrScalar0>();
					struct PaddedA {
						VecDataArray<i, T, Q> a;
						unsigned char padding[sizeof(VecDataArray<L, T, Q>) - sizeof(VecDataArray<i, T, Q>)];
					};
					PaddedA aa = {.a=ctor_mixed_constexpr_single(vecOrScalar0)};
					constexpr std::array<length_t, sizeof...(VecOrScalar)> lengths = { ctor_mixed_constexpr_single_get_length<VecOrScalar>()...};
					const auto params = std::tuple{vecOrScalar...};
					
					const auto arr = ctor_mixed_constexpr_single(std::get<0>(params));
					std::memcpy(aa.a.p.begin()+i, arr, sizeof(T)*lengths[0]);
					constexpr auto i2 = i + lengths[0];
					
					if constexpr (sizeof...(VecOrScalar) > 1) {
						const auto arr2 = ctor_mixed_constexpr_single(std::get<1>(params));
						std::memcpy(aa.a.p.begin()+i2, arr2, sizeof(T)*lengths[1]);
						constexpr auto i3 = i2 + lengths[1];
						if constexpr (sizeof...(VecOrScalar) > 2) {
							const auto arr3 = ctor_mixed_constexpr_single(std::get<2>(params));
							std::memcpy(aa.a.p.begin()+i3, arr3, sizeof(T)*lengths[2]);
						}
					}
					
					return std::bit_cast<data_t>(aa);
				}()
			} {}
			
		// -- Unary arithmetic operators --
		template <length_t Lx, typename Tx, qualifier Qx>
		inline GLM_CONSTEXPR vec<L, T, Q> operator=(vec<Lx, Tx, Qx> v)
		{
			*this = vec<L, T, Q>(v);
			return *this;
		}
		
		
		inline GLM_CONSTEXPR vec<L, T, Q>& operator+=(arithmetic auto scalar)
		{
			if constexpr (L < 3) {
				this->data += scalar;
				return *this;
			} else
				return (*this = detail::compute_vec_add<L, T, Q, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(scalar)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator+=(vec<1, Tx, Q> v) requires (NotVec1<L>)
		{
			if constexpr (L < 3) {
				this->data += v.data;
				return *this;
			} else
				return (*this = detail::compute_vec_add<L, T, Q, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v.x)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator+=(vec<L, Tx, Q> v)
		{
			if constexpr (L < 3) {
				this->data += v.data;
				return *this;
			} else
				return (*this = detail::compute_vec_add<L, T, Q, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v)));
		}

		inline GLM_CONSTEXPR vec<L, T, Q> & operator-=(arithmetic auto scalar)
		{
			if constexpr (L < 3) {
				this->data -= scalar;
				return *this;
			} else
				return (*this = detail::compute_vec_sub<L, T, Q, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(scalar)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator-=(vec<1, Tx, Q> v) requires (NotVec1<L>)
		{
			if constexpr (L < 3) {
				this->data -= v.data;
				return *this;
			} else
				return (*this = detail::compute_vec_sub<L, T, Q, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v.x)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator-=(vec<L, Tx, Q> v)
		{
			if constexpr (L < 3) {
				this->data -= v.data;
				return *this;
			} else
				return (*this = detail::compute_vec_sub<L, T, Q, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v)));
		}

		inline GLM_CONSTEXPR vec<L, T, Q> & operator*=(arithmetic auto scalar)
		{
			if constexpr (L < 3) {
				this->data *= scalar;
				return *this;
			} else
				return (*this = detail::compute_vec_mul<L,T, Q, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(scalar)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator*=(vec<1, Tx, Q> v) requires (NotVec1<L>)
		{
			if constexpr (L < 3) {
				this->data *= v.data;
				return *this;
			} else
				return (*this = detail::compute_vec_mul<L,T, Q, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v.x)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator*=(vec<L, Tx, Q> v)
		{
			if constexpr (L < 3) {
				this->data *= v.data;
				return *this;
			} else
				return (*this = detail::compute_vec_mul<L,T, Q, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v)));
		}

		inline GLM_CONSTEXPR vec<L, T, Q> & operator/=(arithmetic auto scalar)
		{
			if constexpr (L < 3) {
				this->data /= scalar;
				return *this;
			} else
			return (*this = detail::compute_vec_div<L, T, Q, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(scalar)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator/=(vec<1, Tx, Q> v) requires (NotVec1<L>)
		{
			if constexpr (L < 3) {
				this->data /= v.data;
				return *this;
			} else
				return (*this = detail::compute_vec_div<L, T, Q, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v.x)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator/=(vec<L, Tx, Q> v)
		{
			if constexpr (L < 3) {
				this->data /= v.data;
				return *this;
			} else
				return (*this = detail::compute_vec_div<L, T, Q, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v)));
		}
		
		// -- Increment and decrement operators --

		inline GLM_CONSTEXPR vec<L, T, Q> & operator++()
		{
			++this->x;
			++this->y;
			++this->z;
			++this->w;
			return *this;
		}

		inline GLM_CONSTEXPR vec<L, T, Q> & operator--()
		{
			--this->x;
			--this->y;
			--this->z;
			--this->w;
			return *this;
		}

		inline GLM_CONSTEXPR vec<L, T, Q> operator++(int)
		{
			vec<L, T, Q> Result(*this);
			++*this;
			return Result;
		}

		inline GLM_CONSTEXPR vec<L, T, Q> operator--(int)
		{
			vec<L, T, Q> Result(*this);
			--*this;
			return Result;
		}

		// -- Unary bit operators --

		inline GLM_CONSTEXPR vec<L, T, Q> & operator%=(arithmetic auto scalar)
		{
			if constexpr (L < 3) {
				this->data %= scalar;
				return *this;
			} else
				return (*this = detail::compute_vec_mod<L, T, Q, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(scalar)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator%=(vec<1, Tx, Q> v) requires (NotVec1<L>)
		{
			if constexpr (L < 3) {
				this->data *= v.data;
				return *this;
			} else
				return (*this = detail::compute_vec_mod<L, T, Q, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator%=(vec<L, Tx, Q> v)
		{
			if constexpr (L < 3) {
				this->data %= v.data;
				return *this;
			} else
				return (*this = detail::compute_vec_mod<L, T, Q, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v)));
		}

		inline GLM_CONSTEXPR vec<L, T, Q> & operator&=(arithmetic auto scalar)
		{
			if constexpr (L < 3) {
				this->data &= scalar;
				return *this;
			} else
				return (*this = detail::compute_vec_and<L, T, Q, detail::is_int<T>::value, sizeof(T) * 8, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(scalar)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator&=(vec<1, Tx, Q> v) requires (NotVec1<L>)
		{
			if constexpr (L < 3) {
				this->data &= v.data;
				return *this;
			} else
				return (*this = detail::compute_vec_and<L, T, Q, detail::is_int<T>::value, sizeof(T) * 8, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator&=(vec<L, Tx, Q> v)
		{
			if constexpr (L < 3) {
				this->data &= v.data;
				return *this;
			} else
				return (*this = detail::compute_vec_and<L, T, Q, detail::is_int<T>::value, sizeof(T) * 8, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v)));
		}

		inline GLM_CONSTEXPR vec<L, T, Q> & operator|=(arithmetic auto scalar)
		{
			if constexpr (L < 3) {
				this->data |= scalar;
				return *this;
			} else
				return (*this = detail::compute_vec_or<L, T, Q, detail::is_int<T>::value, sizeof(T) * 8, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(scalar)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator|=(vec<1, Tx, Q> const& v) requires (NotVec1<L>)
		{
			if constexpr (L < 3) {
				this->data |= v.data;
				return *this;
			} else
				return (*this = detail::compute_vec_or<L, T, Q, detail::is_int<T>::value, sizeof(T) * 8, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator|=(vec<L, Tx, Q> const& v)
		{
			if constexpr (L < 3) {
				this->data |= v.data;
				return *this;
			} else
				return (*this = detail::compute_vec_or<L, T, Q, detail::is_int<T>::value, sizeof(T) * 8, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v)));
		}

		inline GLM_CONSTEXPR vec<L, T, Q> & operator^=(arithmetic auto scalar)
		{
			if constexpr (L < 3) {
				this->data ^= scalar;
				return *this;
			} else
				return (*this = detail::compute_vec_xor<L, T, Q, detail::is_int<T>::value, sizeof(T) * 8, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(scalar)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator^=(vec<1, Tx, Q> const& v) requires (NotVec1<L>)
		{
			if constexpr (L < 3) {
				this->data ^= v.data;
				return *this;
			} else
				return (*this = detail::compute_vec_xor<L, T, Q, detail::is_int<T>::value, sizeof(T) * 8, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator^=(vec<L, Tx, Q> const& v)
		{
			return (*this = detail::compute_vec_xor<L, T, Q, detail::is_int<T>::value, sizeof(T) * 8, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v)));
		}

		inline GLM_CONSTEXPR vec<L, T, Q> & operator<<=(arithmetic auto scalar)
		{
			if constexpr (L < 3) {
				this->data <<= scalar;
				return *this;
			} else
				return (*this = detail::compute_vec_shift_left<L, T, Q, detail::is_int<T>::value, sizeof(T) * 8, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(scalar)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator<<=(vec<1, Tx, Q> const& v) requires (NotVec1<L>)
		{
			if constexpr (L < 3) {
				this->data <<= v.data;
				return *this;
			} else
				return (*this = detail::compute_vec_shift_left<L, T, Q, detail::is_int<T>::value, sizeof(T) * 8, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator<<=(vec<L, Tx, Q> const& v)
		{
			if constexpr (L < 3) {
				this->data <<= v.data;
				return *this;
			} else
				return (*this = detail::compute_vec_shift_left<L, T, Q, detail::is_int<T>::value, sizeof(T) * 8, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v)));
		}

		inline GLM_CONSTEXPR vec<L, T, Q> & operator>>=(arithmetic auto scalar)
		{
			if constexpr (L < 3) {
				this->data >>= scalar;
				return *this;
			} else
				return (*this = detail::compute_vec_shift_right<L, T, Q, detail::is_int<T>::value, sizeof(T) * 8, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(scalar)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator>>=(vec<1, Tx, Q> const& v) requires (NotVec1<L>)
		{
			if constexpr (L < 3) {
				this->data >>= v.data;
				return *this;
			} else
				return (*this = detail::compute_vec_shift_right<L, T, Q, detail::is_int<T>::value, sizeof(T) * 8, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator>>=(vec<L, Tx, Q> const& v)
		{
			if constexpr (L < 3) {
				this->data >>= v.data;
				return *this;
			} else
				return (*this = detail::compute_vec_shift_right<L, T, Q, detail::is_int<T>::value, sizeof(T) * 8, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v)));
		}

		// -- Unary constant operators --

		inline GLM_CONSTEXPR vec<L, T, Q> operator+()
		{
			return *this;
		}

		inline GLM_CONSTEXPR vec<L, T, Q> operator-()
		{
			return vec<L, T, Q>(0) -= *this;
		}

		// -- Binary arithmetic operators --

		inline GLM_CONSTEXPR vec<L, T, Q> operator+(T scalar)
		{
			return vec<L, T, Q>(*this) += scalar;
		}

		template <length_t Lx>
		inline GLM_CONSTEXPR vec<L, T, Q> operator+(vec<Lx, T, Q> const& v2) requires (!NotVec1<Lx> && NotVec1<L>)
		{
			return vec<L, T, Q>(*this) += v2;
		}

		friend inline GLM_CONSTEXPR vec<L, T, Q> operator+(T scalar, vec<L, T, Q> const& v)
		{
			return vec<L, T, Q>(v) += scalar;
		}

		friend inline GLM_CONSTEXPR vec<L, T, Q> operator+(vec<1, T, Q> const& v1, vec<L, T, Q> const& v2)
		{
			return vec<L, T, Q>(v2) += v1;
		}

		inline GLM_CONSTEXPR vec<L, T, Q> operator+(vec<L, T, Q> v2)
		{
			return vec<L, T, Q>(*this) += v2;
		}

		inline GLM_CONSTEXPR vec<L, T, Q> operator-(T scalar)
		{
			return vec<L, T, Q>(*this) -= scalar;
		}

		template <length_t Lx>
		inline GLM_CONSTEXPR vec<L, T, Q> operator-(vec<Lx, T, Q> const& v2) requires (!NotVec1<Lx> && NotVec1<L>)
		{
			return vec<L, T, Q>(*this) -= v2;
		}

		friend inline GLM_CONSTEXPR vec<L, T, Q> operator-(T scalar, vec<L, T, Q> const& v)
		{
			return vec<L, T, Q>(scalar) -= v;
		}

		friend inline GLM_CONSTEXPR vec<L, T, Q> operator-(vec<1, T, Q> const& v1, vec<L, T, Q> const& v2) requires (NotVec1<L>)
		{
			return vec<L, T, Q>(v1.x) -= v2;
		}

		friend inline GLM_CONSTEXPR vec<L, T, Q> operator-(vec<L, T, Q> const& v1, vec<L, T, Q> const& v2)
		{
			return vec<L, T, Q>(v1) -= v2;
		}

		inline GLM_CONSTEXPR vec<L, T, Q> operator*(T scalar)
		{
			return vec<L, T, Q>(*this) *= scalar;
		}

		template <length_t Lx>
		inline GLM_CONSTEXPR vec<L, T, Q> operator*(vec<Lx, T, Q> const& v2) requires (!NotVec1<Lx> && NotVec1<L>)
		{
			return vec<L, T, Q>(*this) *= v2;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator*(T scalar, vec<L, T, Q> const& v)
		{
			return vec<L, T, Q>(v) *= scalar;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator*(vec<1, T, Q> const& v1, vec<L, T, Q> const& v2) requires (NotVec1<L>)
		{
			return vec<L, T, Q>(v2) *= v1;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator*(vec<L, T, Q> const& v1, vec<L, T, Q> const& v2)
		{
			return vec<L, T, Q>(v1) *= v2;
		}

		
		inline GLM_CONSTEXPR vec<L, T, Q> operator/(T scalar)
		{
			return vec<L, T, Q>(*this) /= scalar;
		}


		template <length_t Lx>
		inline GLM_CONSTEXPR vec<L, T, Q> operator/(vec<Lx, T, Q> const& v2) requires (!NotVec1<Lx> && NotVec1<L>)
		{
			return vec<L, T, Q>(*this) /= v2;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator/(T scalar, vec<L, T, Q> const& v)
		{
			return vec<L, T, Q>(scalar) /= v;
		}

		template <length_t Lx>
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator/(vec<Lx, T, Q> const& v1, vec<L, T, Q> const& v2) requires (!NotVec1<Lx> && NotVec1<L>)
		{
			return vec<L, T, Q>(v1.x) /= v2;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator/(vec<L, T, Q> const& v1, vec<L, T, Q> const& v2)
		{
			return vec<L, T, Q>(v1) /= v2;
		}

		// -- Binary bit operators --

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator%(vec<L, T, Q> const& v, T scalar)
		{
			return vec<L, T, Q>(v) %= scalar;
		}

		template <length_t Lx>
		inline GLM_CONSTEXPR vec<L, T, Q> operator%(vec<Lx, T, Q> const& v2) requires (!NotVec1<Lx> && NotVec1<L>)
		{
			return vec<L, T, Q>(*this) %= v2.x;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator%(T scalar, vec<L, T, Q> const& v)
		{
			return vec<L, T, Q>(scalar) %= v;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator%(vec<1, T, Q> const& scalar, vec<L, T, Q> const& v) requires (NotVec1<L>)
		{
			return vec<L, T, Q>(scalar.x) %= v;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator%(vec<L, T, Q> const& v1, vec<L, T, Q> const& v2)
		{
			return vec<L, T, Q>(v1) %= v2;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator&(vec<L, T, Q> const& v, T scalar)
		{
			return vec<L, T, Q>(v) &= scalar;
		}

		
		inline GLM_CONSTEXPR vec<L, T, Q> operator&(vec<1, T, Q> const& scalar) requires (NotVec1<L>)
		{
			return vec<L, T, Q>(*this) &= scalar;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator&(T scalar, vec<L, T, Q> const& v)
		{
			return vec<L, T, Q>(scalar) &= v;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator&(vec<1, T, Q> const& v1, vec<L, T, Q> const& v2) requires (NotVec1<L>)
		{
			return vec<L, T, Q>(v1.x) &= v2;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator&(vec<L, T, Q> const& v1, vec<L, T, Q> const& v2)
		{
			return vec<L, T, Q>(v1) &= v2;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator|(vec<L, T, Q> const& v, T scalar)
		{
			return vec<L, T, Q>(v) |= scalar;
		}

		template <length_t Lx>
		inline GLM_CONSTEXPR vec<L, T, Q> operator|(vec<Lx, T, Q> const& v2) requires (!NotVec1<Lx> && NotVec1<L>)
		{
			return vec<L, T, Q>(*this) |= v2.x;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator|(T scalar, vec<L, T, Q> const& v)
		{
			return vec<L, T, Q>(scalar) |= v;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator|(vec<1, T, Q> const& v1, vec<L, T, Q> const& v2) requires (NotVec1<L>)
		{
			return vec<L, T, Q>(v1.x) |= v2;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator|(vec<L, T, Q> const& v1, vec<L, T, Q> const& v2)
		{
			return vec<L, T, Q>(v1) |= v2;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator^(vec<L, T, Q> const& v, T scalar)
		{
			return vec<L, T, Q>(v) ^= scalar;
		}

		template <length_t Lx>
		inline GLM_CONSTEXPR vec<L, T, Q> operator^(vec<Lx, T, Q> const& v2) requires (!NotVec1<Lx> && NotVec1<L>)
		{
			return vec<L, T, Q>(*this) ^= v2.x;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator^(T scalar, vec<L, T, Q> const& v)
		{
			return vec<L, T, Q>(scalar) ^= v;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator^(vec<1, T, Q> const& v1, vec<L, T, Q> const& v2) requires (NotVec1<L>)
		{
			return vec<L, T, Q>(v1.x) ^= v2;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator^(vec<L, T, Q> const& v1, vec<L, T, Q> const& v2)
		{
			return vec<L, T, Q>(v1) ^= v2;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator<<(vec<L, T, Q> const& v, T scalar)
		{
			return vec<L, T, Q>(v) <<= scalar;
		}

		template <length_t Lx>
		inline GLM_CONSTEXPR vec<L, T, Q> operator<<(vec<Lx, T, Q> const& v2) requires (!NotVec1<Lx> && NotVec1<L>)
		{
			return vec<L, T, Q>(*this) <<= v2.x;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator<<(T scalar, vec<L, T, Q> const& v)
		{
			return vec<L, T, Q>(scalar) <<= v;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator<<(vec<1, T, Q> const& v1, vec<L, T, Q> const& v2) requires (NotVec1<L>)
		{
			return vec<L, T, Q>(v1.x) <<= v2;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator<<(vec<L, T, Q> const& v1, vec<L, T, Q> const& v2)
		{
			return vec<L, T, Q>(v1) <<= v2;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator>>(vec<L, T, Q> const& v, T scalar)
		{
			return vec<L, T, Q>(v) >>= scalar;
		}

		template <length_t Lx>
		inline GLM_CONSTEXPR vec<L, T, Q> operator>>(vec<Lx, T, Q> const& v2) requires (!NotVec1<Lx> && NotVec1<L>)
		{
			return vec<L, T, Q>(*this) >>= v2.x;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator>>(T scalar, vec<L, T, Q> const& v)
		{
			return vec<L, T, Q>(scalar) >>= v;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator>>(vec<1, T, Q> const& v1, vec<L, T, Q> const& v2) requires (NotVec1<L>)
		{
			return vec<L, T, Q>(v1.x) >>= v2;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator>>(vec<L, T, Q> const& v1, vec<L, T, Q> const& v2)
		{
			return vec<L, T, Q>(v1) >>= v2;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator~(vec<L, T, Q> const& v)
		{
			return detail::compute_vec_bitwise_not<4, T, Q, detail::is_int<T>::value, sizeof(T) * 8, detail::is_aligned<Q>::value>::call(v);
		}

		// -- Boolean operators --
		
		friend inline GLM_CONSTEXPR bool operator==(vec<L, T, Q> const& v1, vec<L, T, Q> const& v2)
		{
			return detail::compute_vec_equal<4, T, Q, detail::is_int<T>::value, sizeof(T) * 8, detail::is_aligned<Q>::value>::call(v1, v2);
		}

		
		friend inline GLM_CONSTEXPR bool operator!=(vec<L, T, Q> const& v1, vec<L, T, Q> const& v2)
		{
			return detail::compute_vec_nequal<4, T, Q, detail::is_int<T>::value, sizeof(T) * 8, detail::is_aligned<Q>::value>::call(v1, v2);
		}
	};
	
	template <length_t Lx, typename Tx, qualifier Qx> requires (std::is_same_v<Tx, bool>)
	inline GLM_CONSTEXPR vec<Lx, bool, Qx> operator&&(vec<Lx, Tx, Qx> const& v1, vec<Lx, Tx, Qx> const& v2)
	{
		return vec<Lx, bool, Qx>(v1.x && v2.x, v1.y && v2.y, v1.z && v2.z, v1.w && v2.w);
	}
	template <length_t Lx, typename Tx, qualifier Qx> requires (std::is_same_v<Tx, bool>)
	inline GLM_CONSTEXPR vec<Lx, bool, Qx> operator||(vec<Lx, bool, Qx> const& v1, vec<Lx, bool, Qx> const& v2)
	{
		return vec<Lx, bool, Qx>(v1.x || v2.x, v1.y || v2.y, v1.z || v2.z, v1.w || v2.w);
	}
}
static_assert( glm::detail::is_aligned<(glm::qualifier)0>::value == false);
static_assert(sizeof(glm::vec<3, float, (glm::qualifier)0>) == 12);