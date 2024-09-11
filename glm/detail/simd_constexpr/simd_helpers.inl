namespace glm::detail
{
	template<length_t L, typename T, qualifier Q>
	struct SimdHelpers
	{
		using PaddedVec = PaddedGccVec<L, T, Q>;
		using gcc_vec_t = PaddedVec::GccV;
		using data_t = typename detail::storage<L, T, detail::is_aligned<Q>::value>::type;
		static inline auto simd_ctor_scalar(arithmetic auto scalar) {
			PaddedVec v = {};
			v.gcc_vec = v.gcc_vec + ( (T)scalar );
			return std::bit_cast<data_t>(v);
		}
		
		template <length_t Lx, typename Tx, qualifier Qx> requires (Lx == L)
		static inline auto simd_ctor(::glm::vec<Lx, Tx, Qx> v)
		{
			using OtherPaddedVec = PaddedGccVec<Lx, Tx, Qx>;
			OtherPaddedVec o = std::bit_cast<OtherPaddedVec>(v.data);
			PaddedVec converted = {.gcc_vec=__builtin_convertvector(o.gcc_vec, gcc_vec_t)};
			return std::bit_cast<data_t>(converted);
		}
		
		template <length_t Lx, typename Tx, qualifier Qx> requires (Lx != L && Lx < L)
		static inline auto simd_ctor(::glm::vec<Lx, Tx, Qx> v)
		{
			using OtherPaddedVec = PaddedGccVec<Lx, Tx, Qx>;
			using OurSizeTheirType = PaddedGccVec<L, Tx, Qx>;
			OtherPaddedVec o = std::bit_cast<OtherPaddedVec>(v.data);
			OurSizeTheirType oExpanded = {};
			for (length_t i = 0; i < Lx; i++) {
				oExpanded.gcc_vec[i] = o.gcc_vec[i];
			}
			
			PaddedVec converted = {.gcc_vec=__builtin_convertvector(oExpanded.gcc_vec, gcc_vec_t)};
			return std::bit_cast<data_t>(converted);
		}
		
		static consteval bool isLengthOfVector(arithmetic auto... scalars) {
			return sizeof...(scalars) == L;
		}
		
		template <arithmetic... A>
		static inline auto simd_ctor_multi_scalars(A... scalars) requires ( isLengthOfVector(scalars...) && SameArithmeticTypes<A...>())
		{
			//assuming that number of scalars is always the same as the length of the to-be-constructed vector
			using OtherPaddedVec = PaddedGccVec<L, A, Q>;
			OtherPaddedVec o = {.gcc_vec={scalars...}};
			PaddedVec converted = {.gcc_vec=__builtin_convertvector(o, gcc_vec_t)};
			return std::bit_cast<data_t>(converted);
		}
	};
}