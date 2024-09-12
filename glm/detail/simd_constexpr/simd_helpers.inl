namespace glm::detail
{
	template<length_t L, typename T, qualifier Q>
	struct SimdHelpers
	{
		template <typename Tx0, typename... Tx>
		struct GetFirstType
		{
			using FirstTx = Tx0;
		};
		template <length_t Lx, typename Tx, qualifier Qx>
		using PaddedVec = PaddedGccVec<Lx, Tx, Qx, detail::BVecNeedsPadding<Lx, Tx, Qx>()>;
		using gcc_vec_t = PaddedVec<L, T, Q>::GccV;
		using data_t = typename detail::storage<L, T, detail::is_aligned<Q>::value>::type;
		
		static inline auto gcc_vec_to_data(PaddedVec<L, T, Q> v) {
			if constexpr (L == 3 && !BIsAlignedQ<Q>()) {
				data_t d;
				std::memcpy(&d, &v, sizeof(d));
				return d;
			} else {
				return std::bit_cast<data_t>(v);
			}
		}
		
		static inline auto simd_ctor_scalar(arithmetic auto scalar) {
			PaddedVec<L, T, Q> v = {};
			v.gcc_vec = v.gcc_vec + ( (T)scalar );
			return gcc_vec_to_data(v);
		}
		
		template <length_t Lx, typename Tx, qualifier Qx> requires (Lx == L)
		static inline auto simd_ctor(::glm::vec<Lx, Tx, Qx> v)
		{
			using OtherPaddedVec = PaddedVec<Lx, Tx, Qx>;
			OtherPaddedVec o = std::bit_cast<OtherPaddedVec>(v.data);
			PaddedVec<L, T, Q> converted = {.gcc_vec=__builtin_convertvector(o.gcc_vec, gcc_vec_t)};
			return gcc_vec_to_data(converted);
		}
		
		template <length_t Lx, typename Tx, qualifier Qx> requires (Lx != L && Lx < L)
		static inline auto simd_ctor(::glm::vec<Lx, Tx, Qx> v)
		{
			using OtherPaddedVec = PaddedVec<Lx, Tx, Qx>;
			using OurSizeTheirType = PaddedVec<L, Tx, Qx>;
			OtherPaddedVec o = std::bit_cast<OtherPaddedVec>(v.data);
			OurSizeTheirType oExpanded = {};
			for (length_t i = 0; i < Lx; i++) {
				oExpanded.gcc_vec[i] = o.gcc_vec[i];
			}
			
			PaddedVec<L, T, Q> converted = {.gcc_vec=__builtin_convertvector(oExpanded.gcc_vec, gcc_vec_t)};
			return gcc_vec_to_data(converted);
		}
		
		template<arithmetic... A>
		static consteval bool isLengthOfVector() {
			return sizeof...(A) == L;
		}
		
		template <arithmetic... A>
		static inline auto simd_ctor_multi_scalars(A... scalars) requires ( isLengthOfVector<A...>() && SameArithmeticTypes<A...>())
		{
			//assuming that number of scalars is always the same as the length of the to-be-constructed vector
			using OtherPaddedVec = PaddedVec<L, typename GetFirstType<A...>::FirstTx, Q>;
			OtherPaddedVec o = {.gcc_vec={scalars...}};
			PaddedVec<L, T, Q> converted = {.gcc_vec=__builtin_convertvector(o.gcc_vec, gcc_vec_t)};
			return gcc_vec_to_data(converted);
		}
	};
}