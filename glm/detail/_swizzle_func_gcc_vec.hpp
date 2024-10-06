#pragma once

#define GLM_SWIZZLE_GEN_VEC2_ENTRY(L, T, Q, CONST, A, B)	\
	GLM_FUNC_QUALIFIER vec<2, T, Q> A ## B() const							\
	{													\
		using E = ElementCollection<L, T, Q>; \
		using G = typename vec<L, T, Q>::GccVec_t; \
		G vin; std::memcpy(&vin, &data, std::min(sizeof(data), sizeof(vin))); \
		GccVec<2, T, Q> vout; \
		vout = __builtin_shufflevector(vin, vin, offsetof(E, A)/sizeof(A), offsetof(E, B)/sizeof(B)); \
		return vec<2, T, Q>(vout);			\
	}

#define GLM_SWIZZLE_GEN_VEC3_ENTRY(L, T, Q, CONST, A, B, C)		\
	GLM_FUNC_QUALIFIER vec<3, T, Q> A ## B ## C() const							\
	{															\
		using E = ElementCollection<L, T, Q>; \
		using G = typename vec<L, T, Q>::GccVec_t; \
		G vin; std::memcpy(&vin, &data, std::min(sizeof(data), sizeof(vin))); \
		GccVec<4, T, Q> vout; \
		vout = __builtin_shufflevector(vin, vin, offsetof(E, A)/sizeof(A), offsetof(E, B)/sizeof(B), offsetof(E, C)/sizeof(C), offsetof(E, A)/sizeof(A)); \
		vec<3, T, Q> voutfin; std::memcpy(&voutfin, &vout, sizeof(voutfin)); \
		return voutfin;			\
	}

#define GLM_SWIZZLE_GEN_VEC4_ENTRY(L, T, Q, CONST, A, B, C, D)					\
	GLM_FUNC_QUALIFIER vec<4, T, Q> A ## B ## C ## D() const									\
	{																		\
		using E = ElementCollection<L, T, Q>; \
		using G = typename vec<L, T, Q>::GccVec_t; \
		G vin; std::memcpy(&vin, &data, std::min(sizeof(data), sizeof(vin))); \
		GccVec<4, T, Q> vout; \
		vout=__builtin_shufflevector(vin, vin, offsetof(E, A)/sizeof(A), offsetof(E, B)/sizeof(B), offsetof(E, C)/sizeof(C), offsetof(E, D)/sizeof(D)); \
		return vec<4, T, Q>(vout);			\
	}

#define GLM_SWIZZLE_GEN_VEC2_ENTRY_DEF(T, Q, L, CONST, A, B)	\
	template<typename T>										\
	GLM_FUNC_QUALIFIER vec<L, T, Q> vec<L, T, Q>::A ## B() const					\
	{															\
		return vec<2, T, Q>(this->A, this->B);					\
	}

#define GLM_SWIZZLE_GEN_VEC3_ENTRY_DEF(T, Q, L, CONST, A, B, C)		\
	template<typename T>											\
	GLM_FUNC_QUALIFIER vec<3, T, Q> vec<L, T, Q>::A ## B ## C() const					\
	{																\
		return vec<3, T, Q>(this->A, this->B, this->C);				\
	}

#define GLM_SWIZZLE_GEN_VEC4_ENTRY_DEF(T, Q, L, CONST, A, B, C, D)		\
	template<typename T>												\
	GLM_FUNC_QUALIFIER vec<4, T, Q> vec<L, T, Q>::A ## B ## C ## D() const					\
	{																	\
		return vec<4, T, Q>(this->A, this->B, this->C, this->D);		\
	}

#define GLM_MUTABLE

#define GLM_SWIZZLE_GEN_VEC2_FROM_VEC2_SWIZZLE(T, Q, A, B) \
	GLM_SWIZZLE_GEN_VEC2_ENTRY(2, T, Q, GLM_MUTABLE, A, B) \
	GLM_SWIZZLE_GEN_VEC2_ENTRY(2, T, Q, GLM_MUTABLE, B, A)

#define GLM_SWIZZLE_GEN_VEC_FROM_VEC2(T, Q) \
	GLM_SWIZZLE_GEN_VEC2_FROM_VEC2_SWIZZLE(T, Q, x, y) \
	GLM_SWIZZLE_GEN_VEC2_FROM_VEC2_SWIZZLE(T, Q, r, g) \
	GLM_SWIZZLE_GEN_VEC2_FROM_VEC2_SWIZZLE(T, Q, s, t)

#define GLM_SWIZZLE_GEN_VEC2_FROM_VEC3_SWIZZLE(T, Q, A, B, C) \
	GLM_SWIZZLE_GEN_VEC2_ENTRY(3, T, Q, GLM_MUTABLE, A, B) \
	GLM_SWIZZLE_GEN_VEC2_ENTRY(3, T, Q, GLM_MUTABLE, A, C) \
	GLM_SWIZZLE_GEN_VEC2_ENTRY(3, T, Q, GLM_MUTABLE, B, A) \
	GLM_SWIZZLE_GEN_VEC2_ENTRY(3, T, Q, GLM_MUTABLE, B, C) \
	GLM_SWIZZLE_GEN_VEC2_ENTRY(3, T, Q, GLM_MUTABLE, C, A) \
	GLM_SWIZZLE_GEN_VEC2_ENTRY(3, T, Q, GLM_MUTABLE, C, B)

#define GLM_SWIZZLE_GEN_VEC3_FROM_VEC3_SWIZZLE(T, Q, A, B, C) \
	GLM_SWIZZLE_GEN_VEC3_ENTRY(3, T, Q, GLM_MUTABLE, A, B, C) \
	GLM_SWIZZLE_GEN_VEC3_ENTRY(3, T, Q, GLM_MUTABLE, A, C, B) \
	GLM_SWIZZLE_GEN_VEC3_ENTRY(3, T, Q, GLM_MUTABLE, B, A, C) \
	GLM_SWIZZLE_GEN_VEC3_ENTRY(3, T, Q, GLM_MUTABLE, B, C, A) \
	GLM_SWIZZLE_GEN_VEC3_ENTRY(3, T, Q, GLM_MUTABLE, C, A, B) \
	GLM_SWIZZLE_GEN_VEC3_ENTRY(3, T, Q, GLM_MUTABLE, C, B, A)

#define GLM_SWIZZLE_GEN_VEC_FROM_VEC3_COMQ(T, Q, A, B, C) \
	GLM_SWIZZLE_GEN_VEC3_FROM_VEC3_SWIZZLE(T, Q, A, B, C) \
	GLM_SWIZZLE_GEN_VEC2_FROM_VEC3_SWIZZLE(T, Q, A, B, C)

#define GLM_SWIZZLE_GEN_VEC_FROM_VEC3(T, Q) \
	GLM_SWIZZLE_GEN_VEC_FROM_VEC3_COMQ(T, Q, x, y, z) \
	GLM_SWIZZLE_GEN_VEC_FROM_VEC3_COMQ(T, Q, r, g, b) \
	GLM_SWIZZLE_GEN_VEC_FROM_VEC3_COMQ(T, Q, s, t, p)

#define GLM_SWIZZLE_GEN_VEC2_FROM_VEC4_SWIZZLE(T, Q, A, B, C, D) \
	GLM_SWIZZLE_GEN_VEC2_ENTRY(4, T, Q, GLM_MUTABLE, A, B) \
	GLM_SWIZZLE_GEN_VEC2_ENTRY(4, T, Q, GLM_MUTABLE, A, C) \
	GLM_SWIZZLE_GEN_VEC2_ENTRY(4, T, Q, GLM_MUTABLE, A, D) \
	GLM_SWIZZLE_GEN_VEC2_ENTRY(4, T, Q, GLM_MUTABLE, B, A) \
	GLM_SWIZZLE_GEN_VEC2_ENTRY(4, T, Q, GLM_MUTABLE, B, C) \
	GLM_SWIZZLE_GEN_VEC2_ENTRY(4, T, Q, GLM_MUTABLE, B, D) \
	GLM_SWIZZLE_GEN_VEC2_ENTRY(4, T, Q, GLM_MUTABLE, C, A) \
	GLM_SWIZZLE_GEN_VEC2_ENTRY(4, T, Q, GLM_MUTABLE, C, B) \
	GLM_SWIZZLE_GEN_VEC2_ENTRY(4, T, Q, GLM_MUTABLE, C, D) \
	GLM_SWIZZLE_GEN_VEC2_ENTRY(4, T, Q, GLM_MUTABLE, D, A) \
	GLM_SWIZZLE_GEN_VEC2_ENTRY(4, T, Q, GLM_MUTABLE, D, B) \
	GLM_SWIZZLE_GEN_VEC2_ENTRY(4, T, Q, GLM_MUTABLE, D, C)

#define GLM_SWIZZLE_GEN_VEC3_FROM_VEC4_SWIZZLE(T, Q, A, B, C, D) \
	GLM_SWIZZLE_GEN_VEC3_ENTRY(4, T, Q, , A, B, C) \
	GLM_SWIZZLE_GEN_VEC3_ENTRY(4, T, Q, , A, B, D) \
	GLM_SWIZZLE_GEN_VEC3_ENTRY(4, T, Q, , A, C, B) \
	GLM_SWIZZLE_GEN_VEC3_ENTRY(4, T, Q, , A, C, D) \
	GLM_SWIZZLE_GEN_VEC3_ENTRY(4, T, Q, , A, D, B) \
	GLM_SWIZZLE_GEN_VEC3_ENTRY(4, T, Q, , A, D, C) \
	GLM_SWIZZLE_GEN_VEC3_ENTRY(4, T, Q, , B, A, C) \
	GLM_SWIZZLE_GEN_VEC3_ENTRY(4, T, Q, , B, A, D) \
	GLM_SWIZZLE_GEN_VEC3_ENTRY(4, T, Q, , B, C, A) \
	GLM_SWIZZLE_GEN_VEC3_ENTRY(4, T, Q, , B, C, D) \
	GLM_SWIZZLE_GEN_VEC3_ENTRY(4, T, Q, , B, D, A) \
	GLM_SWIZZLE_GEN_VEC3_ENTRY(4, T, Q, , B, D, C) \
	GLM_SWIZZLE_GEN_VEC3_ENTRY(4, T, Q, , C, A, B) \
	GLM_SWIZZLE_GEN_VEC3_ENTRY(4, T, Q, , C, A, D) \
	GLM_SWIZZLE_GEN_VEC3_ENTRY(4, T, Q, , C, B, A) \
	GLM_SWIZZLE_GEN_VEC3_ENTRY(4, T, Q, , C, B, D) \
	GLM_SWIZZLE_GEN_VEC3_ENTRY(4, T, Q, , C, D, A) \
	GLM_SWIZZLE_GEN_VEC3_ENTRY(4, T, Q, , C, D, B) \
	GLM_SWIZZLE_GEN_VEC3_ENTRY(4, T, Q, , D, A, B) \
	GLM_SWIZZLE_GEN_VEC3_ENTRY(4, T, Q, , D, A, C) \
	GLM_SWIZZLE_GEN_VEC3_ENTRY(4, T, Q, , D, B, A) \
	GLM_SWIZZLE_GEN_VEC3_ENTRY(4, T, Q, , D, B, C) \
	GLM_SWIZZLE_GEN_VEC3_ENTRY(4, T, Q, , D, C, A) \
	GLM_SWIZZLE_GEN_VEC3_ENTRY(4, T, Q, , D, C, B)

#define GLM_SWIZZLE_GEN_VEC4_FROM_VEC4_SWIZZLE(T, Q, A, B, C, D) \
	GLM_SWIZZLE_GEN_VEC4_ENTRY(4, T, Q, , A, C, B, D) \
	GLM_SWIZZLE_GEN_VEC4_ENTRY(4, T, Q, , A, C, D, B) \
	GLM_SWIZZLE_GEN_VEC4_ENTRY(4, T, Q, , A, D, B, C) \
	GLM_SWIZZLE_GEN_VEC4_ENTRY(4, T, Q, , A, D, C, B) \
	GLM_SWIZZLE_GEN_VEC4_ENTRY(4, T, Q, , A, B, D, C) \
	GLM_SWIZZLE_GEN_VEC4_ENTRY(4, T, Q, , A, B, C, D) \
	GLM_SWIZZLE_GEN_VEC4_ENTRY(4, T, Q, , B, C, A, D) \
	GLM_SWIZZLE_GEN_VEC4_ENTRY(4, T, Q, , B, C, D, A) \
	GLM_SWIZZLE_GEN_VEC4_ENTRY(4, T, Q, , B, D, A, C) \
	GLM_SWIZZLE_GEN_VEC4_ENTRY(4, T, Q, , B, D, C, A) \
	GLM_SWIZZLE_GEN_VEC4_ENTRY(4, T, Q, , B, A, D, C) \
	GLM_SWIZZLE_GEN_VEC4_ENTRY(4, T, Q, , B, A, C, D) \
	GLM_SWIZZLE_GEN_VEC4_ENTRY(4, T, Q, , C, B, A, D) \
	GLM_SWIZZLE_GEN_VEC4_ENTRY(4, T, Q, , C, B, D, A) \
	GLM_SWIZZLE_GEN_VEC4_ENTRY(4, T, Q, , C, D, A, B) \
	GLM_SWIZZLE_GEN_VEC4_ENTRY(4, T, Q, , C, D, B, A) \
	GLM_SWIZZLE_GEN_VEC4_ENTRY(4, T, Q, , C, A, D, B) \
	GLM_SWIZZLE_GEN_VEC4_ENTRY(4, T, Q, , C, A, B, D) \
	GLM_SWIZZLE_GEN_VEC4_ENTRY(4, T, Q, , D, C, B, A) \
	GLM_SWIZZLE_GEN_VEC4_ENTRY(4, T, Q, , D, C, A, B) \
	GLM_SWIZZLE_GEN_VEC4_ENTRY(4, T, Q, , D, A, B, C) \
	GLM_SWIZZLE_GEN_VEC4_ENTRY(4, T, Q, , D, A, C, B) \
	GLM_SWIZZLE_GEN_VEC4_ENTRY(4, T, Q, , D, B, A, C) \
	GLM_SWIZZLE_GEN_VEC4_ENTRY(4, T, Q, , D, B, C, A)

#define GLM_SWIZZLE_GEN_VEC_FROM_VEC4_COMQ(T, Q, A, B, C, D) \
	GLM_SWIZZLE_GEN_VEC2_FROM_VEC4_SWIZZLE(T, Q, A, B, C, D) \
	GLM_SWIZZLE_GEN_VEC3_FROM_VEC4_SWIZZLE(T, Q, A, B, C, D) \
	GLM_SWIZZLE_GEN_VEC4_FROM_VEC4_SWIZZLE(T, Q, A, B, C, D)

#define GLM_SWIZZLE_GEN_VEC_FROM_VEC4(T, Q) \
	GLM_SWIZZLE_GEN_VEC_FROM_VEC4_COMQ(T, Q, x, y, z, w) \
	GLM_SWIZZLE_GEN_VEC_FROM_VEC4_COMQ(T, Q, r, g, b, a) \
	GLM_SWIZZLE_GEN_VEC_FROM_VEC4_COMQ(T, Q, s, t, p, q)
