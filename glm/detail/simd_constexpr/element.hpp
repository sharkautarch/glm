#include <iostream>
namespace glm::detail
{
	consteval bool NotEmpty(length_t I, length_t L) { return I <= L; }
	struct Empty {};
	struct  GLM_TRIVIAL RowTwo {
		[[no_unique_address]] Empty y; [[no_unique_address]] Empty g; [[no_unique_address]] Empty t;
	};
	struct GLM_TRIVIAL RowThree {
		[[no_unique_address]] Empty z; [[no_unique_address]] Empty b; [[no_unique_address]] Empty p;
	};
	struct GLM_TRIVIAL RowFour {
		[[no_unique_address]] Empty w; [[no_unique_address]] Empty a; [[no_unique_address]] Empty q;
	};
	template <qualifier Q, typename T, length_t L>
	struct ElementCollection;
	template <qualifier Q, typename T>
	struct  GLM_TRIVIAL ElementCollection<Q, T, 4> {
		using data_t = typename detail::storage<4, T, detail::is_aligned<Q>::value>::type;
		union
		{
				struct {
					union { T x, r, s; }; 
					union { T y, g, t; };
					union { T z, b, p; };
					union { T w, a, q; };
				};
				data_t data;
		};
	};
	

	template <qualifier Q, typename T>
	struct GLM_TRIVIAL ElementCollection<Q, T, 3> : RowFour {
		using data_t = typename detail::storage<3, T, detail::is_aligned<Q>::value>::type;
		using RowFour::w;
		using RowFour::a;
		using RowFour::q;
		union
		{
				struct {
					union { T x, r, s; }; 
					union { T y, g, t; };
					union { T z, b, p; };
				};
				data_t data;
		};
	};
	template <qualifier Q, typename T>
	struct GLM_TRIVIAL ElementCollection<Q, T, 2> : RowThree, RowFour {
		using data_t = typename detail::storage<2, T, detail::is_aligned<Q>::value>::type;
		using RowThree::z;
		using RowThree::b;
		using RowThree::p;
		using RowFour::w;
		using RowFour::a;
		using RowFour::q;
		union
		{
				struct {
					union { T x, r, s; }; 
					union { T y, g, t; };
				};
				data_t data;
		};
	};
	template <qualifier Q, typename T>
	struct GLM_TRIVIAL ElementCollection<Q, T, 1> : RowTwo, RowThree, RowFour {
		using data_t = typename detail::storage<1, T, detail::is_aligned<Q>::value>::type;
		using RowTwo::y;
		using RowTwo::g;
		using RowTwo::t;
		using RowThree::z;
		using RowThree::b;
		using RowThree::p;
		using RowFour::w;
		using RowFour::a;
		using RowFour::q;
		union
		{
				struct {
					union { T x, r, s; }; 
				};
				data_t data;
		};
		#undef G
	};
}

