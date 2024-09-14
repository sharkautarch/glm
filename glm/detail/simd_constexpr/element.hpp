#include <iostream>
namespace glm::detail
{
	consteval bool NotEmpty(length_t I, length_t L) { return I <= L; }
	template <qualifier Q, typename T, length_t L>
	struct ElementCollection;
	template <qualifier Q, typename T>
	struct ElementCollection<Q, T, 4> {
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
	struct ElementCollection<Q, T, 3> {
		using data_t = typename detail::storage<3, T, detail::is_aligned<Q>::value>::type;
		struct w {}; struct a {}; struct q{};
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
	struct ElementCollection<Q, T, 2> {
		using data_t = typename detail::storage<2, T, detail::is_aligned<Q>::value>::type;
		struct z {}; struct b {}; struct p{};
		struct w {}; struct a {}; struct q{};
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
	struct ElementCollection<Q, T, 1> {
		using data_t = typename detail::storage<1, T, detail::is_aligned<Q>::value>::type;
		struct y {}; struct g {}; struct t{};
		struct z {}; struct b {}; struct p{};
		struct w {}; struct a {}; struct q{};
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

