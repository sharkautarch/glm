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
namespace glm
{
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
		return (std::is_same_v<T0, T> && ...);
	}
	template <typename... T>
	consteval bool NotSameArithmeticTypes() {
		return ( (!(std::is_integral_v<T> || std::is_floating_point_v<T>) || ...) || !(SameArithmeticTypes<T...>()) );
	}
	
	namespace detail
	{
		template <length_t L, typename T, qualifier Q>
		using ArrT = T[L];

		template <length_t L, typename T, qualifier Q>
		using _data_t = typename detail::storage<L, T, detail::is_aligned<Q>::value>::type;
		
		template <length_t L, typename T, qualifier Q>
		using GccV = T __attribute__(( vector_size(sizeof(T)*L), aligned(alignof(_data_t<L, T, Q>)) ));
		
		template <length_t L, typename T, qualifier Q>
		consteval bool BDataNeedsPadding() {
			return sizeof(_data_t<L,T,Q>) > sizeof(ArrT<L,T,Q>);
		}
		template <length_t L, typename T, qualifier Q>
		consteval bool BVecNeedsPadding() {
			return sizeof(_data_t<L,T,Q>) > sizeof(GccV<L,T,Q>);
		}
		template <length_t L, typename T, qualifier Q, bool NeedsPadding>
		struct VecDataArray;
			
		template <length_t L, typename T, qualifier Q>
		struct VecDataArray<L, T, Q, true> {
			using ArrT = ArrT<L, T, Q>;
			using data_t = _data_t<L,T,Q>;
			ArrT p;
			std::byte padding[sizeof(data_t) - sizeof(ArrT)];
		};
		template <length_t L, typename T, qualifier Q>
		struct VecDataArray<L, T, Q, false> {
			using ArrT = ArrT<L, T, Q>;
			ArrT p;
		};
		
		template <length_t L, typename T, qualifier Q, bool NeedsPadding>
		struct PaddedGccVec;
		
		template <length_t L, typename T, qualifier Q>
		struct PaddedGccVec<L, T, Q, true> {
			using GccV = GccV<L, T,Q>;
			using data_t = _data_t<L, T, Q>;
			GccV gcc_vec;
			std::byte padding[sizeof(data_t) - sizeof(GccV)];
		};
	
		template <length_t L, typename T, qualifier Q>
		struct PaddedGccVec<L, T, Q, false> {
			using GccV = GccV<L, T,Q>;
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
namespace glm
{
	template<length_t L, typename T, qualifier Q>
	struct vec
	{
		using SimdHlp = detail::SimdHelpers<L, T, Q>;
		using DataArray = VecDataArray<L, T, Q>;
		using data_t = typename detail::storage<L, T, detail::is_aligned<Q>::value>::type;

		// -- Implementation detail --
		typedef T value_type;
		typedef vec<L, T, Q> type;
		typedef vec<L, bool, Q> bool_type;
		static constexpr qualifier k_qual = Q;
		
		enum is_aligned
		{
			value = detail::is_aligned<Q>::value
		};

		// -- Component Access --
		static consteval length_t length(){	return L;	}
		
		inline GLM_CONSTEXPR T& operator[](length_t i)
		{
			if (!std::is_constant_evaluated()) {
				GLM_ASSERT_LENGTH(i, L);
			}
			switch (std::max(i, length()))
			{
				default:
				case 0:
					return x;
				case 1:
					return y.t;
				case 2:
					return z.t;
				case 3:
					return w.t;
			}
		}
		
		
		// -- Data --
#define GLM_N [[no_unique_address]]
		template <length_t I>
		using E = detail::Element<detail::NotEmpty(I,L),T,I,L>;
		union
		{
				struct {
					union       {          T x,            r,            s; };
					GLM_N union { GLM_N E<2> y; GLM_N E<2> g; GLM_N E<2> t; };
					GLM_N union { GLM_N E<3> z; GLM_N E<3> b; GLM_N E<3> p; };
					GLM_N union { GLM_N E<4> w; GLM_N E<4> a; GLM_N E<4> q; };
				};
				data_t data;
		};
#undef GLM_N
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
		constexpr auto ctor_scalar(ScalarGetter scalar) {
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
		constexpr auto ctor(VecGetter vecGetter) {
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
		
		typedef struct {
			DataArray a;
			length_t i;
		} RetPair;
		static inline auto ctor_mixed_constexpr_single = []<typename var_t>(auto&& vs0, length_t index) -> var_t
		{
			DataArray a {};
			using VTX = std::decay_t<decltype(vs0)>;
			length_t i = 0;
			auto&& __restrict__ _vs0 = vs0;
			if constexpr ( std::is_integral_v<VTX> || std::is_floating_point_v<VTX> ) {
				a.p[index] = _vs0;
				i++;
			} else {
				using Tx = VTX::value_type;
				using ArrX = VecDataArray<_vs0.length(), Tx, VTX::k_qual>;
				ArrX ax = std::bit_cast<ArrX>(_vs0.data);
				for (Tx tx : ax.p) {
					a.p[index+i++] = (T)tx;
				}
			}
			
			return var_t{RetPair{a, i}};
		};
		
		constexpr vec() : data{} {}
		constexpr vec(arithmetic auto scalar) : data{ [scalar,this](){ auto s = [scalar](){ return scalar; }; return ctor_scalar(s); }() } {}
		
		template <length_t Lx, typename Tx, qualifier Qx>
		constexpr vec(vec<Lx, Tx, Qx> v) : data{ [v, this](){ auto vv = [v](){ return v; }; return ctor(vv); }() } {} 
		
		template <arithmetic... Scalar> requires (sizeof...(Scalar) == L)
		constexpr vec(Scalar... scalar)
		: data
			{ [scalar...]() -> data_t
				{
					if (std::is_constant_evaluated()) {
						DataArray a = {.p={ T(scalar)... }};
						return std::bit_cast<data_t>(a);
					} else {
						return SimdHlp::simd_ctor_multi_scalars(scalar...);
					}
				}()
			} {}
			
		template <typename... VecOrScalar> requires (sizeof...(VecOrScalar) > 1 && NotSameArithmeticTypes<VecOrScalar...>())
		constexpr vec(VecOrScalar... vecOrScalar)
		: data
			{ [vecOrScalar...]() -> data_t
				{
					//type_vecx.inl never had any simd versions for ctor from mixes of scalars & vectors,
					//so I don't really need to figure out how I'd make a generic simd version for this ctor 
					DataArray a {};
					length_t i = 0;
					using var_t = std::variant<RetPair, VecOrScalar...>;
					for (auto var_vs : std::array<var_t, sizeof...(vecOrScalar)>{ vecOrScalar... } ) {
						auto visitee = [i](auto&& arg) -> var_t { return ctor_mixed_constexpr_single.template operator()<var_t>(arg, i); };
						RetPair pair = std::get<RetPair>(std::visit(visitee, var_vs));
						for (length_t j = pair.i; j < i+pair.i; j++) {
							a.p[j] = pair.a.p[j];
						}
						i+=pair.i;
					}
					
					return std::bit_cast<data_t>(a);
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
			return (*this = detail::compute_vec_add<L, T, Q, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(scalar)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator+=(vec<1, Tx, Q> v)
		{
			return (*this = detail::compute_vec_add<L, T, Q, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v.x)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator+=(vec<L, Tx, Q> v)
		{
			return (*this = detail::compute_vec_add<L, T, Q, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v)));
		}

		inline GLM_CONSTEXPR vec<L, T, Q> & operator-=(arithmetic auto scalar)
		{
			return (*this = detail::compute_vec_sub<L, T, Q, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(scalar)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator-=(vec<1, Tx, Q> v)
		{
			return (*this = detail::compute_vec_sub<L, T, Q, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v.x)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator-=(vec<L, Tx, Q> v)
		{
			return (*this = detail::compute_vec_sub<L, T, Q, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v)));
		}

		inline GLM_CONSTEXPR vec<L, T, Q> & operator*=(arithmetic auto scalar)
		{
			return (*this = detail::compute_vec_mul<L,T, Q, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(scalar)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator*=(vec<1, Tx, Q> v)
		{
			return (*this = detail::compute_vec_mul<L,T, Q, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v.x)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator*=(vec<L, Tx, Q> v)
		{
			return (*this = detail::compute_vec_mul<L,T, Q, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v)));
		}

		inline GLM_CONSTEXPR vec<L, T, Q> & operator/=(arithmetic auto scalar)
		{
			return (*this = detail::compute_vec_div<L, T, Q, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(scalar)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator/=(vec<1, Tx, Q> v)
		{
			return (*this = detail::compute_vec_div<L, T, Q, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v.x)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator/=(vec<L, Tx, Q> v)
		{
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
			return (*this = detail::compute_vec_mod<L, T, Q, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(scalar)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator%=(vec<1, Tx, Q> v)
		{
			return (*this = detail::compute_vec_mod<L, T, Q, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator%=(vec<L, Tx, Q> v)
		{
			return (*this = detail::compute_vec_mod<L, T, Q, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v)));
		}

		inline GLM_CONSTEXPR vec<L, T, Q> & operator&=(arithmetic auto scalar)
		{
			return (*this = detail::compute_vec_and<L, T, Q, detail::is_int<T>::value, sizeof(T) * 8, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(scalar)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator&=(vec<1, Tx, Q> v)
		{
			return (*this = detail::compute_vec_and<L, T, Q, detail::is_int<T>::value, sizeof(T) * 8, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator&=(vec<L, Tx, Q> v)
		{
			return (*this = detail::compute_vec_and<L, T, Q, detail::is_int<T>::value, sizeof(T) * 8, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v)));
		}

		inline GLM_CONSTEXPR vec<L, T, Q> & operator|=(arithmetic auto scalar)
		{
			return (*this = detail::compute_vec_or<L, T, Q, detail::is_int<T>::value, sizeof(T) * 8, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(scalar)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator|=(vec<1, Tx, Q> const& v)
		{
			return (*this = detail::compute_vec_or<L, T, Q, detail::is_int<T>::value, sizeof(T) * 8, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator|=(vec<L, Tx, Q> const& v)
		{
			return (*this = detail::compute_vec_or<L, T, Q, detail::is_int<T>::value, sizeof(T) * 8, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v)));
		}

		inline GLM_CONSTEXPR vec<L, T, Q> & operator^=(arithmetic auto scalar)
		{
			return (*this = detail::compute_vec_xor<L, T, Q, detail::is_int<T>::value, sizeof(T) * 8, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(scalar)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator^=(vec<1, Tx, Q> const& v)
		{
			return (*this = detail::compute_vec_xor<L, T, Q, detail::is_int<T>::value, sizeof(T) * 8, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator^=(vec<L, Tx, Q> const& v)
		{
			return (*this = detail::compute_vec_xor<L, T, Q, detail::is_int<T>::value, sizeof(T) * 8, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v)));
		}

		inline GLM_CONSTEXPR vec<L, T, Q> & operator<<=(arithmetic auto scalar)
		{
			return (*this = detail::compute_vec_shift_left<L, T, Q, detail::is_int<T>::value, sizeof(T) * 8, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(scalar)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator<<=(vec<1, Tx, Q> const& v)
		{
			return (*this = detail::compute_vec_shift_left<L, T, Q, detail::is_int<T>::value, sizeof(T) * 8, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator<<=(vec<L, Tx, Q> const& v)
		{
			return (*this = detail::compute_vec_shift_left<L, T, Q, detail::is_int<T>::value, sizeof(T) * 8, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v)));
		}

		inline GLM_CONSTEXPR vec<L, T, Q> & operator>>=(arithmetic auto scalar)
		{
			return (*this = detail::compute_vec_shift_right<L, T, Q, detail::is_int<T>::value, sizeof(T) * 8, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(scalar)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator>>=(vec<1, Tx, Q> const& v)
		{
			return (*this = detail::compute_vec_shift_right<L, T, Q, detail::is_int<T>::value, sizeof(T) * 8, detail::is_aligned<Q>::value>::call(*this, vec<L, T, Q>(v)));
		}

		template<typename Tx>
		inline GLM_CONSTEXPR vec<L, T, Q> & operator>>=(vec<L, Tx, Q> const& v)
		{
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

		inline GLM_CONSTEXPR vec<L, T, Q> operator+(vec<1, T, Q> const& v2)
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

		inline GLM_CONSTEXPR vec<L, T, Q> operator-(vec<1, T, Q> const& v2)
		{
			return vec<L, T, Q>(*this) -= v2;
		}

		friend inline GLM_CONSTEXPR vec<L, T, Q> operator-(T scalar, vec<L, T, Q> const& v)
		{
			return vec<L, T, Q>(scalar) -= v;
		}

		friend inline GLM_CONSTEXPR vec<L, T, Q> operator-(vec<1, T, Q> const& v1, vec<L, T, Q> const& v2)
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

		
		inline GLM_CONSTEXPR vec<L, T, Q> operator*(vec<1, T, Q> const& v2)
		{
			return vec<L, T, Q>(*this) *= v2;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator*(T scalar, vec<L, T, Q> const& v)
		{
			return vec<L, T, Q>(v) *= scalar;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator*(vec<1, T, Q> const& v1, vec<L, T, Q> const& v2)
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

		
		inline GLM_CONSTEXPR vec<L, T, Q> operator/(vec<1, T, Q> const& v2)
		{
			return vec<L, T, Q>(*this) /= v2;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator/(T scalar, vec<L, T, Q> const& v)
		{
			return vec<L, T, Q>(scalar) /= v;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator/(vec<1, T, Q> const& v1, vec<L, T, Q> const& v2)
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

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator%(vec<L, T, Q> const& v1, vec<1, T, Q> const& v2)
		{
			return vec<L, T, Q>(v1) %= v2.x;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator%(T scalar, vec<L, T, Q> const& v)
		{
			return vec<L, T, Q>(scalar) %= v;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator%(vec<1, T, Q> const& scalar, vec<L, T, Q> const& v)
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

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator&(vec<L, T, Q> const& v, vec<1, T, Q> const& scalar)
		{
			return vec<L, T, Q>(v) &= scalar;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator&(T scalar, vec<L, T, Q> const& v)
		{
			return vec<L, T, Q>(scalar) &= v;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator&(vec<1, T, Q> const& v1, vec<L, T, Q> const& v2)
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

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator|(vec<L, T, Q> const& v1, vec<1, T, Q> const& v2)
		{
			return vec<L, T, Q>(v1) |= v2.x;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator|(T scalar, vec<L, T, Q> const& v)
		{
			return vec<L, T, Q>(scalar) |= v;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator|(vec<1, T, Q> const& v1, vec<L, T, Q> const& v2)
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

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator^(vec<L, T, Q> const& v1, vec<1, T, Q> const& v2)
		{
			return vec<L, T, Q>(v1) ^= v2.x;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator^(T scalar, vec<L, T, Q> const& v)
		{
			return vec<L, T, Q>(scalar) ^= v;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator^(vec<1, T, Q> const& v1, vec<L, T, Q> const& v2)
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

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator<<(vec<L, T, Q> const& v1, vec<1, T, Q> const& v2)
		{
			return vec<L, T, Q>(v1) <<= v2.x;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator<<(T scalar, vec<L, T, Q> const& v)
		{
			return vec<L, T, Q>(scalar) <<= v;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator<<(vec<1, T, Q> const& v1, vec<L, T, Q> const& v2)
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

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator>>(vec<L, T, Q> const& v1, vec<1, T, Q> const& v2)
		{
			return vec<L, T, Q>(v1) >>= v2.x;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator>>(T scalar, vec<L, T, Q> const& v)
		{
			return vec<L, T, Q>(scalar) >>= v;
		}

		
		friend inline GLM_CONSTEXPR vec<L, T, Q> operator>>(vec<1, T, Q> const& v1, vec<L, T, Q> const& v2)
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