/// @ref gtx_fast_exponential

namespace glm
{
	namespace detail 
	{
#	if (GLM_LANG & GLM_LANG_CXX20_FLAG)
		template <typename To, typename From>
		constexpr To GLM_ATTR_2 bit_cast(From const& src) noexcept
		{
			return std::bit_cast<To>(src);
		}
		
#	else
		template <typename To, typename From>
		constexpr To GLM_ATTR_2 bit_cast(From const& src) noexcept
		{
			To dst;
			std::memcpy(&dst, &src, sizeof(To));
			return dst;
		}
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////
//	SSE2 pow2f4 approximation funcs are MIT Licensed																								/
//	Copyright (2008) José Fonseca																																		/
//	originally from:https://jrfonseca.blogspot.com/2008/09/fast-sse2-pow-tables-or-polynomials.html /
/////////////////////////////////////////////////////////////////////////////////////////////////////

#		define EXP_POLY_DEGREE 3

#		define POLY0(x, c0) _mm_set1_ps(c0)
#		define POLY1(x, c0, c1) _mm_add_ps(_mm_mul_ps(POLY0(x, c1), x), _mm_set1_ps(c0))
#		define POLY2(x, c0, c1, c2) _mm_add_ps(_mm_mul_ps(POLY1(x, c1, c2), x), _mm_set1_ps(c0))
#		define POLY3(x, c0, c1, c2, c3) _mm_add_ps(_mm_mul_ps(POLY2(x, c1, c2, c3), x), _mm_set1_ps(c0))
#		define POLY4(x, c0, c1, c2, c3, c4) _mm_add_ps(_mm_mul_ps(POLY3(x, c1, c2, c3, c4), x), _mm_set1_ps(c0))
#		define POLY5(x, c0, c1, c2, c3, c4, c5) _mm_add_ps(_mm_mul_ps(POLY4(x, c1, c2, c3, c4, c5), x), _mm_set1_ps(c0))

#		define GCCV_POLY0(v, c0) decltype((v))(c0)
#		define GCCV_POLY1(v, c0, c1) (GCCV_POLY0(v, c1))*v + GCCV_POLY0(v,c0)
#if 0
#		define POLY2(x, c0, c1, c2) (POLY1(v, c1, c2)) * v + (GCCV_POLY0(v, c0))
#		define POLY3(x, c0, c1, c2, c3) _mm_add_ps(_mm_mul_ps(POLY2(x, c1, c2, c3), x), _mm_set1_ps(c0))
#		define POLY4(x, c0, c1, c2, c3, c4) _mm_add_ps(_mm_mul_ps(POLY3(x, c1, c2, c3, c4), x), _mm_set1_ps(c0))
#		define POLY5(x, c0, c1, c2, c3, c4, c5) _mm_add_ps(_mm_mul_ps(POLY4(x, c1, c2, c3, c4, c5), x), _mm_set1_ps(c0))
#endif

		static inline __m128 GLM_ATTR_2 exp2f4(__m128 x)
		{
			 __m128i ipart;
			 __m128 fpart, expipart, expfpart;

			 x = _mm_min_ps(x, _mm_set1_ps( 129.00000f));
			 x = _mm_max_ps(x, _mm_set1_ps(-126.99999f));

			 /* ipart = int(x - 0.5) */
			 ipart = _mm_cvtps_epi32(_mm_sub_ps(x, _mm_set1_ps(0.5f)));

			 /* fpart = x - ipart */
			 fpart = _mm_sub_ps(x, _mm_cvtepi32_ps(ipart));

			 /* expipart = (float) (1 << ipart) */
			 expipart = _mm_castsi128_ps(_mm_slli_epi32(_mm_add_epi32(ipart, _mm_set1_epi32(127)), 23));

			 /* minimax polynomial fit of 2**x, in range [-0.5, 0.5[ */
		#if EXP_POLY_DEGREE == 5
			 expfpart = POLY5(fpart, 9.9999994e-1f, 6.9315308e-1f, 2.4015361e-1f, 5.5826318e-2f, 8.9893397e-3f, 1.8775767e-3f);
		#elif EXP_POLY_DEGREE == 4
			 expfpart = POLY4(fpart, 1.0000026f, 6.9300383e-1f, 2.4144275e-1f, 5.2011464e-2f, 1.3534167e-2f);
		#elif EXP_POLY_DEGREE == 3
			 expfpart = POLY3(fpart, 9.9992520e-1f, 6.9583356e-1f, 2.2606716e-1f, 7.8024521e-2f);
		#elif EXP_POLY_DEGREE == 2
			 expfpart = POLY2(fpart, 1.0017247f, 6.5763628e-1f, 3.3718944e-1f);
		#else
		#error
		#endif

			 return _mm_mul_ps(expipart, expfpart);
		}

#		define LOG_POLY_DEGREE 5
#if 0
		template <length_t L, qualifier Q>
		static inline vec<L, float, Q> log2f4(vec<L, float, Q> v)
		{
			
		}
#endif
		static inline __m128 GLM_ATTR_2 log2f4(__m128 x)
		{
			 __m128i exp = _mm_set1_epi32(0x7F800000);
			 __m128i mant = _mm_set1_epi32(0x007FFFFF);

			 __m128 one = _mm_set1_ps( 1.0f);

			 __m128i i = _mm_castps_si128(x);

			 __m128 e = _mm_cvtepi32_ps(_mm_sub_epi32(_mm_srli_epi32(_mm_and_si128(i, exp), 23), _mm_set1_epi32(127)));

			 __m128 m = _mm_or_ps(_mm_castsi128_ps(_mm_and_si128(i, mant)), one);

			 __m128 p;

			 /* Minimax polynomial fit of log2(x)/(x - 1), for x in range [1, 2[ */
		#if LOG_POLY_DEGREE == 6
			 p = POLY5( m, 3.1157899f, -3.3241990f, 2.5988452f, -1.2315303f,  3.1821337e-1f, -3.4436006e-2f);
		#elif LOG_POLY_DEGREE == 5
			 p = POLY4(m, 2.8882704548164776201f, -2.52074962577807006663f, 1.48116647521213171641f, -0.465725644288844778798f, 0.0596515482674574969533f);
		#elif LOG_POLY_DEGREE == 4
			 p = POLY3(m, 2.61761038894603480148f, -1.75647175389045657003f, 0.688243882994381274313f, -0.107254423828329604454f);
		#elif LOG_POLY_DEGREE == 3
			 p = POLY2(m, 2.28330284476918490682f, -1.04913055217340124191f, 0.204446009836232697516f);
		#else
		#error
		#endif

			 /* This effectively increases the polynomial degree by one, but ensures that log2(1) == 0*/
			 p = _mm_mul_ps(p, _mm_sub_ps(m, one));

			 return _mm_add_ps(p, e);
		}
	
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//	End Of SSE2 pow2f4 approximation funcs																													/
/////////////////////////////////////////////////////////////////////////////////////////////////////

	// fastPow:
	template<typename genType>
	GLM_FUNC_QUALIFIER genType fastPow(genType x, genType y)
	{
		return exp(y * log(x));
	}

	template<typename T, qualifier Q>
	GLM_FUNC_QUALIFIER vec<1, T, Q> fastPow(vec<1, T, Q> const& x, vec<1, T, Q> const& y)
	{
		return exp(y * log(x));
	}
	template<typename T, qualifier Q>
	GLM_FUNC_QUALIFIER vec<2, T, Q> fastPow(vec<2, T, Q> const& x, vec<2, T, Q> const& y)
	{
		return exp(y * log(x));
	}
	
	template<qualifier Q>
	GLM_FUNC_QUALIFIER vec<3, float, Q> GLM_ATTR fastPow(vec<3, float, Q> const& x, vec<3, float, Q> const& y)
	{
		return vec<3, float, Q>(  vec<4, float, Q>(detail::exp2f4( detail::bit_cast<__m128>(vec<4, float, Q>(y))) * detail::log2f4(detail::bit_cast<__m128>(vec<4, float, Q> (x)))) );
	}
	
	template<qualifier Q>
	GLM_FUNC_QUALIFIER vec<4, float, Q> GLM_ATTR fastPow(vec<4, float, Q> const& x, vec<4, float, Q> const& y)
	{
		return vec<4, float, Q>(  vec<4, float, Q>(detail::exp2f4( detail::bit_cast<__m128>(vec<4, float, Q>(y))) * detail::log2f4(detail::bit_cast<__m128>(vec<4, float, Q> (x)))) );
	}

	template<typename T>
	GLM_FUNC_QUALIFIER T fastPow(T x, int y)
	{
		T f = static_cast<T>(1);
		for(int i = 0; i < y; ++i)
			f *= x;
		return f;
	}

	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_QUALIFIER vec<L, T, Q> fastPow(vec<L, T, Q> const& x, vec<L, int, Q> const& y)
	{
		vec<L, T, Q> Result;
		for(length_t i = 0, n = x.length(); i < n; ++i)
			Result[i] = fastPow(x[i], y[i]);
		return Result;
	}

	// fastExp
	// Note: This function provides accurate results only for value between -1 and 1, else avoid it.
	template<typename T>
	GLM_FUNC_QUALIFIER T fastExp(T x)
	{
		// This has a better looking and same performance in release mode than the following code. However, in debug mode it's slower.
		// return 1.0f + x * (1.0f + x * 0.5f * (1.0f + x * 0.3333333333f * (1.0f + x * 0.25 * (1.0f + x * 0.2f))));
		T x2 = x * x;
		T x3 = x2 * x;
		T x4 = x3 * x;
		T x5 = x4 * x;
		return T(1) + x + (x2 * T(0.5)) + (x3 * T(0.1666666667)) + (x4 * T(0.041666667)) + (x5 * T(0.008333333333));
	}
	/*  // Try to handle all values of float... but often shower than std::exp, glm::floor and the loop kill the performance
	GLM_FUNC_QUALIFIER float fastExp(float x)
	{
		const float e = 2.718281828f;
		const float IntegerPart = floor(x);
		const float FloatPart = x - IntegerPart;
		float z = 1.f;

		for(int i = 0; i < int(IntegerPart); ++i)
			z *= e;

		const float x2 = FloatPart * FloatPart;
		const float x3 = x2 * FloatPart;
		const float x4 = x3 * FloatPart;
		const float x5 = x4 * FloatPart;
		return z * (1.0f + FloatPart + (x2 * 0.5f) + (x3 * 0.1666666667f) + (x4 * 0.041666667f) + (x5 * 0.008333333333f));
	}

	// Increase accuracy on number bigger that 1 and smaller than -1 but it's not enough for high and negative numbers
	GLM_FUNC_QUALIFIER float fastExp(float x)
	{
		// This has a better looking and same performance in release mode than the following code. However, in debug mode it's slower.
		// return 1.0f + x * (1.0f + x * 0.5f * (1.0f + x * 0.3333333333f * (1.0f + x * 0.25 * (1.0f + x * 0.2f))));
		float x2 = x * x;
		float x3 = x2 * x;
		float x4 = x3 * x;
		float x5 = x4 * x;
		float x6 = x5 * x;
		float x7 = x6 * x;
		float x8 = x7 * x;
		return 1.0f + x + (x2 * 0.5f) + (x3 * 0.1666666667f) + (x4 * 0.041666667f) + (x5 * 0.008333333333f)+ (x6 * 0.00138888888888f) + (x7 * 0.000198412698f) + (x8 * 0.0000248015873f);;
	}
	*/

	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_QUALIFIER vec<L, T, Q> fastExp(vec<L, T, Q> const& x)
	{
		return detail::functor1<vec, L, T, T, Q>::call(fastExp, x);
	}

	// fastLog
	template<typename genType>
	GLM_FUNC_QUALIFIER genType fastLog(genType x)
	{
		return std::log(x);
	}

	/* Slower than the VC7.1 function...
	GLM_FUNC_QUALIFIER float fastLog(float x)
	{
		float y1 = (x - 1.0f) / (x + 1.0f);
		float y2 = y1 * y1;
		return 2.0f * y1 * (1.0f + y2 * (0.3333333333f + y2 * (0.2f + y2 * 0.1428571429f)));
	}
	*/

	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_QUALIFIER vec<L, T, Q> fastLog(vec<L, T, Q> const& x)
	{
		return detail::functor1<vec, L, T, T, Q>::call(fastLog, x);
	}

	//fastExp2, ln2 = 0.69314718055994530941723212145818f
	template<typename genType>
	GLM_FUNC_QUALIFIER genType fastExp2(genType x)
	{
		return fastExp(static_cast<genType>(0.69314718055994530941723212145818) * x);
	}

	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_QUALIFIER vec<L, T, Q> fastExp2(vec<L, T, Q> const& x)
	{
		return detail::functor1<vec, L, T, T, Q>::call(fastExp2, x);
	}

	// fastLog2, ln2 = 0.69314718055994530941723212145818f
	template<typename genType>
	GLM_FUNC_QUALIFIER genType fastLog2(genType x)
	{
		return fastLog(x) / static_cast<genType>(0.69314718055994530941723212145818);
	}

	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_QUALIFIER vec<L, T, Q> fastLog2(vec<L, T, Q> const& x)
	{
		return detail::functor1<vec, L, T, T, Q>::call(fastLog2, x);
	}
}//namespace glm
