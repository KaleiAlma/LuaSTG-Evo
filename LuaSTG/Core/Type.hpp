#pragma once
#include <cstdint>
#include <cmath>
#include <limits>
#include <string_view>

namespace Core
{
	// 二维向量

	template<typename T>
	struct Vector2
	{
		T x{};
		T y{};

		Vector2() noexcept : x(0), y(0) {}
		Vector2(T const x_, T const y_) noexcept : x(x_), y(y_) {}

		inline Vector2 operator+() const noexcept { return this; }
		inline Vector2 operator-() const noexcept { return Vector2(-x, -y); }

		inline Vector2 operator+(Vector2 const& r) const noexcept { return Vector2(x + r.x, y + r.y); }
		inline Vector2 operator-(Vector2 const& r) const noexcept { return Vector2(x - r.x, y - r.y); }
		inline Vector2 operator+(T const r) const noexcept { return Vector2(x + r, y + r); }
		inline Vector2 operator-(T const r) const noexcept { return Vector2(x - r, y - r); }
		inline Vector2 operator*(Vector2 const& r) const noexcept { return Vector2(x * r.x, y * r.y); }
		inline Vector2 operator/(Vector2 const& r) const noexcept { return Vector2(x / r.x, y / r.y); }
		inline Vector2 operator*(T const r) const noexcept { return Vector2(x * r, y * r); }
		inline Vector2 operator/(T const r) const noexcept { return Vector2(x / r, y / r); }
		friend inline Vector2 operator/(T const r, Vector2 const& v) noexcept { return Vector2(r / v.x, r / v.y); }

		inline Vector2& operator+=(Vector2 const& r) noexcept { x += r.x; y += r.y; return *this; }
		inline Vector2& operator-=(Vector2 const& r) noexcept { x -= r.x; y -= r.y; return *this; }
		inline Vector2& operator+=(T const r) noexcept { x += r; y += r; return *this; }
		inline Vector2& operator-=(T const r) noexcept { x -= r; y -= r; return *this; }
		inline Vector2& operator*=(Vector2 const& r) noexcept { x *= r.x; y *= r.y; return *this; }
		inline Vector2& operator/=(Vector2 const& r) noexcept { x /= r.x; y /= r.y; return *this; }
		inline Vector2& operator*=(T const r) noexcept { x *= r; y *= r; return *this; }
		inline Vector2& operator/=(T const r) noexcept { x /= r; y /= r; return *this; }

		inline bool operator==(Vector2 const& r) const noexcept { return x == r.x && y == r.y; }
		inline bool operator!=(Vector2 const& r) const noexcept { return x != r.x || y != r.y; }

		inline T& operator[](size_t const i) { return (&x)[i]; }
		inline T& operator[](size_t const i) const { return (&x)[i]; }

		inline float dot(Vector2 const& r) const noexcept { return x * r.x + y * r.y; }

		inline Vector2& normalize() noexcept
		{
			T const l = length();
			if (l >= std::numeric_limits<T>::min())
			{
				x /= l; y /= l;
			}
			else
			{
				x = T{}; y = T{};
			}
			return *this;
		}
		inline Vector2 normalized() const noexcept
		{
			T const l = length();
			if (l >= std::numeric_limits<T>::min())
			{
				return Vector2(x / l, y / l);
			}
			else
			{
				return Vector2();
			}
		}
		inline T length() const noexcept { return std::sqrt(x * x + y * y); }
		inline T angle() const noexcept { return std::atan2(y, x); }
	};

	using Vector2I = Vector2<int32_t>;
	using Vector2U = Vector2<uint32_t>;
	using Vector2F = Vector2<float>;

	// 表示一个长方形区域

	template<typename T>
	struct Rect
	{
		Vector2<T> a;
		Vector2<T> b;

		Rect() noexcept {}
		Rect(Vector2<T> const& a_, Vector2<T> const& b_) noexcept : a(a_), b(b_) {}
		Rect(T const left, T const top, T const right, T const bottom) noexcept : a(left, top), b(right, bottom) {}

		inline bool operator==(Rect const& r) const noexcept { return a == r.a && b == r.b; }
		inline bool operator!=(Rect const& r) const noexcept { return a != r.a || b != r.b; }

		inline Rect operator+(Vector2<T> const& r) const noexcept { return Rect(a + r, b + r); }
		inline Rect operator-(Vector2<T> const& r) const noexcept { return Rect(a - r, b - r); }

		inline T width() const noexcept { return b.x - a.x; }
		inline T height() const noexcept { return b.y - a.y; }
		inline Vector2<T> dim() const noexcept { return b - a; }

		//inline Rect operator*(T const r) const noexcept { return Rect(a * r, b * r); }
	};

	using RectI = Rect<int32_t>;
	using RectU = Rect<uint32_t>;
	using RectF = Rect<float>;

	// 三维向量

	template<typename T>
	struct Vector3
	{
		T x{};
		T y{};
		T z{};

		Vector3() noexcept : x(0), y(0), z(0) {}
		Vector3(T const x_, T const y_, T const z_) noexcept : x(x_), y(y_), z(z_) {}

		inline Vector3 operator+() const noexcept { return this; }
		inline Vector3 operator-() const noexcept { return Vector3(-x, -y, -z); }

		inline Vector3 operator+(Vector3 const& r) const noexcept { return Vector3(x + r.x, y + r.y, z + r.z); }
		inline Vector3 operator-(Vector3 const& r) const noexcept { return Vector3(x - r.x, y - r.y, z - r.z); }
		inline Vector3 operator+(T const r) const noexcept { return Vector3(x + r, y + r, z + r); }
		inline Vector3 operator-(T const r) const noexcept { return Vector3(x - r, y - r, z - r); }
		inline Vector3 operator*(Vector3 const& r) const noexcept { return Vector3(x * r.x, y * r.y, z * r.z); }
		inline Vector3 operator/(Vector3 const& r) const noexcept { return Vector3(x / r.x, y / r.y, z / r.z); }
		inline Vector3 operator*(T const r) const noexcept { return Vector3(x * r, y * r, z * r); }
		inline Vector3 operator/(T const r) const noexcept { return Vector3(x / r, y / r, z / r); }
		friend inline Vector3 operator/(T const r, Vector3 const& v) noexcept { return Vector3(r / v.x, r / v.y, r / v.z); }

		inline Vector3& operator+=(Vector3 const& r) noexcept { x += r.x; y += r.y; z += r.z; return *this; }
		inline Vector3& operator-=(Vector3 const& r) noexcept { x -= r.x; y -= r.y; z -= r.z; return *this; }
		inline Vector3& operator+=(T const r) noexcept { x += r; y += r; z += r; return *this; }
		inline Vector3& operator-=(T const r) noexcept { x -= r; y -= r; z -= r; return *this; }
		inline Vector3& operator*=(Vector3 const& r) noexcept { x *= r.x; y *= r.y; z *= r.z; return *this; }
		inline Vector3& operator/=(Vector3 const& r) noexcept { x /= r.x; y /= r.y; z /= r.z; return *this; }
		inline Vector3& operator*=(T const r) noexcept { x *= r; y *= r; z *= r; return *this; }
		inline Vector3& operator/=(T const r) noexcept { x /= r; y /= r; z /= r; return *this; }

		inline bool operator==(Vector3 const& r) const noexcept { return x == r.x && y == r.y && z == r.z; }
		inline bool operator!=(Vector3 const& r) const noexcept { return x != r.x || y != r.y || z != r.z; }

		inline T& operator[](size_t const i) { return (&x)[i]; }
		inline T& operator[](size_t const i) const { return (&x)[i]; }

		inline float dot(Vector3 const& r) const noexcept { return x * r.x + y * r.y + z * r.z; }

		inline Vector3& normalize() noexcept
		{
			T const l = length();
			if (l >= std::numeric_limits<T>::min())
			{
				x /= l; y /= l; z /= l;
			}
			else
			{
				x = T{}; y = T{}; z = T{};
			}
			return *this;
		}
		inline Vector3 normalized() const noexcept
		{
			T const l = length();
			if (l >= std::numeric_limits<T>::min())
			{
				return Vector3(x / l, y / l, z / l);
			}
			else
			{
				return Vector3();
			}
		}
		inline T length() const noexcept { return std::sqrt(x * x + y * y + z * z); }
	};

	using Vector3I = Vector3<int32_t>;
	using Vector3U = Vector3<uint32_t>;
	using Vector3F = Vector3<float>;

	// 表示一个长方体区域

	template<typename T>
	struct Box
	{
		Vector3<T> a;
		Vector3<T> b;

		Box() noexcept {}
		Box(Vector3<T> const& a_, Vector3<T> const& b_) noexcept : a(a_), b(b_) {}
		Box(T const left, T const top, T const front, T const right, T const bottom, T const back) noexcept : a(left, top, front), b(right, bottom, back) {}

		inline bool operator==(Box const& r) const noexcept { return a == r.a && b == r.b; }
		inline bool operator!=(Box const& r) const noexcept { return a != r.a || b != r.b; }
	};

	using BoxI = Box<int32_t>;
	using BoxU = Box<uint32_t>;
	using BoxF = Box<float>;

	// 四维向量

	template<typename T>
	struct Vector4
	{
		T x{};
		T y{};
		T z{};
		T w{};

		Vector4() noexcept : x(0), y(0), z(0), w(0) {}
		Vector4(T const x_, T const y_, T const z_, T const w_) noexcept : x(x_), y(y_), z(z_), w(w_) {}

		inline Vector4 operator+() const noexcept { return this; }
		inline Vector4 operator-() const noexcept { return Vector4(-x, -y, -z, -w); }

		inline Vector4 operator+(Vector4 const& r) const noexcept { return Vector4(x + r.x, y + r.y, z + r.z, w + r.w); }
		inline Vector4 operator-(Vector4 const& r) const noexcept { return Vector4(x - r.x, y - r.y, z - r.z, w - r.w); }
		inline Vector4 operator+(T const r) const noexcept { return Vector4(x + r, y + r, z + r, w + r); }
		inline Vector4 operator-(T const r) const noexcept { return Vector4(x - r, y - r, z - r, w - r); }
		inline Vector4 operator*(Vector4 const& r) const noexcept { return Vector4(x * r.x, y * r.y, z * r.z, w * r.w); }
		inline Vector4 operator/(Vector4 const& r) const noexcept { return Vector4(x / r.x, y / r.y, z / r.z, w / r.w); }
		inline Vector4 operator*(T const r) const noexcept { return Vector4(x * r, y * r, z * r, w * r); }
		inline Vector4 operator/(T const r) const noexcept { return Vector4(x / r, y / r, z / r, w / r); }
		friend inline Vector4 operator/(T const r, Vector4 const& v) noexcept { return Vector4(r / v.x, r / v.y, r / v.z, r / v.w); }

		inline Vector4& operator+=(Vector4 const& r) noexcept { x += r.x; y += r.y; z += r.z; w += r.w; return *this; }
		inline Vector4& operator-=(Vector4 const& r) noexcept { x -= r.x; y -= r.y; z -= r.z; w -= r.w; return *this; }
		inline Vector4& operator+=(T const r) noexcept { x += r; y += r; z += r; w += r; return *this; }
		inline Vector4& operator-=(T const r) noexcept { x -= r; y -= r; z -= r; w -= r; return *this; }
		inline Vector4& operator*=(Vector4 const& r) noexcept { x *= r.x; y *= r.y; z *= r.z; w *= r.w; return *this; }
		inline Vector4& operator/=(Vector4 const& r) noexcept { x /= r.x; y /= r.y; z /= r.z; w /= r.w; return *this; }
		inline Vector4& operator*=(T const r) noexcept { x *= r; y *= r; z *= r; w *= r; return *this; }
		inline Vector4& operator/=(T const r) noexcept { x /= r; y /= r; z /= r; w /= r; return *this; }

		inline bool operator==(Vector4 const& r) const noexcept { return x == r.x && y == r.y && z == r.z && w == r.w; }
		inline bool operator!=(Vector4 const& r) const noexcept { return x != r.x || y != r.y || z != r.z || w != r.w; }

		inline T& operator[](size_t const i) { return (&x)[i]; }
		inline T& operator[](size_t const i) const { return (&x)[i]; }


		inline float dot(Vector4 const& r) const noexcept { return x * r.x + y * r.y + z * r.z + w * r.w; }

		inline Vector4& normalize() noexcept
		{
			T const l = length();
			if (l >= std::numeric_limits<T>::min())
			{
				x /= l; y /= l; z /= l; w /= l;
			}
			else
			{
				x = T{}; y = T{}; z = T{}; w = T{};
			}
			return *this;
		}
		inline Vector4 normalized() const noexcept
		{
			T const l = length();
			if (l >= std::numeric_limits<T>::min())
			{
				return Vector4(x / l, y / l, z / l, w / l);
			}
			else
			{
				return Vector4();
			}
		}
		inline T length() const noexcept { return std::sqrt(x * x + y * y + z * z + w * w); }
	};

	using Vector4I = Vector4<int32_t>;
	using Vector4U = Vector4<uint32_t>;
	using Vector4F = Vector4<float>;

	template<typename T>
	struct Matrix2
	{
		using vec = Vector2<T>;

		vec value[2]{ vec(), vec() };

		Matrix2() noexcept {}
		Matrix2(vec const v1, vec const v2) noexcept : value{v1, v2} {}
		Matrix2(T const x1, T const x2, T const y1, T const y2) noexcept : value{{x1, y1}, {x2, y2}} {}

		static inline Matrix2 identity()
		{
			return Matrix2<T>(
				1, 0,
				0, 1
			);
		}

		inline Matrix2 operator+() const noexcept { return this; }
		inline Matrix2 operator-() const noexcept { return Matrix2(-value[0], -value[1]); }

		inline Matrix2 operator+(Matrix2 const& r) const noexcept { return Matrix2(value[0] + r[0], value[1] + r[1]); }
		inline Matrix2 operator-(Matrix2 const& r) const noexcept { return Matrix2(value[0] - r[0], value[1] - r[1]); }
		inline Matrix2 operator+(T const r) const noexcept { return Matrix2(value[0] + r, value[1] + r); }
		inline Matrix2 operator-(T const r) const noexcept { return Matrix2(value[0] - r, value[1] - r); }

		inline Matrix2 operator*(Matrix2 const& r) const noexcept { return Matrix2(value[0] * r[0].x + value[1] * r[0].y, value[0] * r[1].x + value[1] * r[1].y); }
		inline Matrix2 operator/(Matrix2 const& r) const noexcept { return *this * r.inverse(); }
		inline vec operator*(vec const& r) const noexcept { return value[0] * r.x + value[1] * r.y; }
		inline vec operator/(vec const& r) const noexcept { return this->inverse() * r; }
		friend inline vec operator*(vec const& r, Matrix2 const& m) noexcept { return vec(m[0][0] * r.x + m[0][1] * r.y, m[1][0] * r.x + m[1][1] * r.y); }
		friend inline vec operator/(vec const& r, Matrix2 const& m) noexcept { return r * m.inverse(); }
		inline Matrix2 operator*(T const r) const noexcept { return Matrix2(value[0] * r, value[1] * r); }
		inline Matrix2 operator/(T const r) const noexcept { return Matrix2(value[0] / r, value[1] / r); }
		friend inline Matrix2 operator/(T const r, Matrix2 const& m) noexcept { return Matrix2(r / m[0], r / m[1]); }

		inline Matrix2& operator+=(Matrix2 const& r) noexcept { value[0] += r[0]; value[1] += r[1]; return *this; }
		inline Matrix2& operator-=(Matrix2 const& r) noexcept { value[0] -= r[0]; value[1] -= r[1]; return *this; }
		inline Matrix2& operator+=(T const r) noexcept { value[0] += r; value[1] += r; return *this; }
		inline Matrix2& operator-=(T const r) noexcept { value[0] -= r; value[1] -= r; return *this; }
		inline Matrix2& operator*=(Matrix2 const r) noexcept { *this = *this * r; return *this; }
		inline Matrix2& operator/=(Matrix2 const r) noexcept { *this *= r.inverse(); return *this; }
		inline Matrix2& operator*=(T const r) noexcept { value[0] *= r; value[1] *= r; return *this; }
		inline Matrix2& operator/=(T const r) noexcept { value[0] /= r; value[1] /= r; return *this; }

		inline bool operator==(Matrix2 const& r) const noexcept { return value[0] == r[0] && value[1] == r[1]; }
		inline bool operator!=(Matrix2 const& r) const noexcept { return !(*this == r); }

		inline vec& operator[](size_t const i) { return value[i]; }
		inline vec& operator[](size_t const i) const { return value[i]; }

		inline Matrix2 inverse() const noexcept
		{
			T inverse_determinant = 1 / determinant();

			return Matrix2(
				+value[1][1] * inverse_determinant,
				-value[0][1] * inverse_determinant,
				-value[1][0] * inverse_determinant,
				+value[0][0] * inverse_determinant
			);
		}
		inline T determinant() const noexcept
		{
			return value[0][0] * value[1][1] - value[1][0] * value[0][1];
		}
		inline Matrix2 transpose() const noexcept
		{
			return Matrix2(
				value[0][0],
				value[1][0],
				value[0][1],
				value[1][1]
			);
		}
	};

	using Matrix2I = Matrix2<int32_t>;
	using Matrix2U = Matrix2<uint32_t>;
	using Matrix2F = Matrix2<float>;

	template<typename T>
	struct Matrix3
	{
		using vec = Vector3<T>;

		vec value[3]{ vec(), vec(), vec() };

		Matrix3() noexcept {}
		Matrix3(vec const v1, vec const v2, vec const v3) noexcept : value{v1, v2, v3} {}
		Matrix3(T const x1, T const y1, T const z1, T const x2, T const y2, T const z2, T const x3, T const y3, T const z3) noexcept
		: value{{x1, y1, z1}, {x2, y2, z2}, {x3, y3, z3}} {}

		static inline Matrix3 identity()
		{
			return Matrix3<T>(
				1, 0, 0,
				0, 1, 0,
				0, 0, 1
			);
		}

		inline Matrix3 operator+() const noexcept { return this; }
		inline Matrix3 operator-() const noexcept { return Matrix3(-value[0], -value[1], -value[2]); }

		inline Matrix3 operator+(Matrix3 const& r) const noexcept { return Matrix3(value[0] + r[0], value[1] + r[1], value[2] + r[2]); }
		inline Matrix3 operator-(Matrix3 const& r) const noexcept { return Matrix3(value[0] - r[0], value[1] - r[1], value[2] - r[2]); }
		inline Matrix3 operator+(T const r) const noexcept { return Matrix3(value[0] + r, value[1] + r, value[2] + r); }
		inline Matrix3 operator-(T const r) const noexcept { return Matrix3(value[0] - r, value[1] - r, value[2] - r); }

		inline Matrix3 operator*(Matrix3 const& r) const noexcept { return Matrix3(value[0] * r[0].x + value[1] * r[0].y + value[2] * r[0].z, value[0] * r[1].x + value[1] * r[1].y + value[2] * r[1].z, value[0] * r[2].x + value[1] * r[2].y + value[2] * r[2].z); }
		inline Matrix3 operator/(Matrix3 const& r) const noexcept { return *this * r.inverse(); }
		inline vec operator*(vec const& r) const noexcept { return value[0] * r.x + value[1] * r.y + value[2] * r.z; }
		inline vec operator/(vec const& r) const noexcept { return this->inverse() * r; }
		friend inline vec operator*(vec const& r, Matrix3 const& m) noexcept { return vec(m[0].dot(r.x), m[1].dot(r.y), m[2].dot(r.z)); }
		friend inline vec operator/(vec const& r, Matrix3 const& m) noexcept { return r * m.inverse(); }
		inline Matrix3 operator*(T const r) const noexcept { return Matrix3(value[0] * r, value[1] * r, value[2] * r); }
		inline Matrix3 operator/(T const r) const noexcept { return Matrix3(value[0] / r, value[1] / r, value[2] / r); }
		friend inline Matrix3 operator/(T const r, Matrix3 const& m) noexcept { return Matrix3(r / m[0], r / m[1], r / m[2]); }

		inline Matrix3& operator+=(Matrix3 const& r) noexcept { value[0] += r[0]; value[1] += r[1]; value[2] += r[2]; return *this; }
		inline Matrix3& operator-=(Matrix3 const& r) noexcept { value[0] -= r[0]; value[1] -= r[1]; value[2] -= r[2]; return *this; }
		inline Matrix3& operator+=(T const r) noexcept { value[0] += r; value[1] += r; value[2] += r; return *this; }
		inline Matrix3& operator-=(T const r) noexcept { value[0] -= r; value[1] -= r; value[2] -= r; return *this; }
		inline Matrix3& operator*=(Matrix3 const r) noexcept { *this = *this * r; return *this; }
		inline Matrix3& operator/=(Matrix3 const r) noexcept { *this *= r.inverse(); return *this; }
		inline Matrix3& operator*=(T const r) noexcept { value[0] *= r; value[1] *= r; value[2] *= r; return *this; }
		inline Matrix3& operator/=(T const r) noexcept { value[0] /= r; value[1] /= r; value[2] /= r; return *this; }

		inline bool operator==(Matrix3 const& r) const noexcept { return value[0] == r[0] && value[1] == r[1] && value[2] == r[2]; }
		inline bool operator!=(Matrix3 const& r) const noexcept { return !(*this == r); }

		inline vec& operator[](size_t const i) { return value[i]; }
		inline vec& operator[](size_t const i) const { return value[i]; }

		inline Matrix3 inverse() const noexcept
		{
			T inverse_determinant = 1 / determinant();

			return Matrix3(
				+(value[1][1] * value[2][2] - value[2][1] * value[1][2]),
				-(value[1][0] * value[2][2] - value[2][0] * value[1][2]),
				+(value[1][0] * value[2][1] - value[2][0] * value[1][1]),

				-(value[0][1] * value[2][2] - value[2][1] * value[0][2]),
				+(value[0][0] * value[2][2] - value[2][0] * value[0][2]),
				-(value[0][0] * value[2][1] - value[2][0] * value[0][1]),
				
				+(value[0][1] * value[1][2] - value[1][1] * value[0][2]),
				-(value[0][0] * value[1][2] - value[1][0] * value[0][2]),
				+(value[0][0] * value[1][1] - value[1][0] * value[0][1])
			) * inverse_determinant;
		}
		inline T determinant() const noexcept
		{
			return
				+value[0][0] * (value[1][1] * value[2][2] - value[2][1] * value[1][2])
				-value[1][0] * (value[0][1] * value[2][2] - value[2][1] * value[0][2])
				+value[2][0] * (value[0][1] * value[1][2] - value[1][1] * value[0][2]);
		}
		inline Matrix3 transpose() const noexcept
		{
			return Matrix3(
				value[0][0],
				value[1][0],
				value[2][0],
				value[0][1],
				value[1][1],
				value[2][1],
				value[0][2],
				value[1][2],
				value[2][2]
			);
		}
	};

	using Matrix3I = Matrix3<int32_t>;
	using Matrix3U = Matrix3<uint32_t>;
	using Matrix3F = Matrix3<float>;

	template<typename T>
	struct Matrix4
	{
		using vec = Vector4<T>;

		vec value[4]{ vec(), vec(), vec() };

		Matrix4() noexcept {}
		Matrix4(vec const v1, vec const v2, vec const v3) noexcept : value{v1, v2, v3} {}
		Matrix4(T const x1, T const y1, T const z1, T const w1, T const x2, T const y2, T const z2, T const w2, T const x3, T const y3, T const z3, T const w3, T const x4, T const y4, T const z4, T const w4) noexcept
		: value{{x1, y1, z1, w1}, {x2, y2, z2, w2}, {x3, y3, z3, w3}, {x4, y4, z4, w4}} {}

		static inline Matrix4 identity()
		{
			return Matrix4<T>(
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1
			);
		}

		inline Matrix4 operator+() const noexcept { return this; }
		inline Matrix4 operator-() const noexcept { return Matrix4(-value[0], -value[1], -value[2], -value[3]); }

		inline Matrix4 operator+(Matrix4 const& r) const noexcept { return Matrix4(value[0] + r[0], value[1] + r[1], value[2] + r[2], value[3] + r[3]); }
		inline Matrix4 operator-(Matrix4 const& r) const noexcept { return Matrix4(value[0] - r[0], value[1] - r[1], value[2] - r[2], value[3] - r[3]); }
		inline Matrix4 operator+(T const r) const noexcept { return Matrix4(value[0] + r, value[1] + r, value[2] + r, value[3] + r); }
		inline Matrix4 operator-(T const r) const noexcept { return Matrix4(value[0] - r, value[1] - r, value[2] - r, value[3] - r); }

		inline Matrix4 operator*(Matrix4 const& r) const noexcept { return Matrix4(value[0] * r[0].x + value[1] * r[0].y + value[2] * r[0].z + value[3] * r[0].w, value[0] * r[1].x + value[1] * r[1].y + value[2] * r[1].z + value[3] * r[1].w, value[0] * r[2].x + value[1] * r[2].y + value[2] * r[2].z + value[3] * r[2].w, value[0] * r[3].x + value[1] * r[3].y + value[2] * r[3].z + value[3] * r[3].w); }
		inline Matrix4 operator/(Matrix4 const& r) const noexcept { return *this * r.inverse(); }
		inline vec operator*(vec const& r) const noexcept { return value[0] * r.x + value[1] * r.y + value[2] * r.z + value[3] * r.w; }
		inline vec operator/(vec const& r) const noexcept { return this->inverse() * r; }
		friend inline vec operator*(vec const& r, Matrix4 const& m) noexcept { return vec(m[0].dot(r.x), m[1].dot(r.y), m[2].dot(r.z), m[3].dot(r.w)); }
		friend inline vec operator/(vec const& r, Matrix4 const& m) noexcept { return r * m.inverse(); }
		inline Matrix4 operator*(T const r) const noexcept { return Matrix4(value[0] * r, value[1] * r, value[2] * r, value[3] * r); }
		inline Matrix4 operator/(T const r) const noexcept { return Matrix4(value[0] / r, value[1] / r, value[2] / r, value[3] / r); }
		friend inline Matrix4 operator/(T const r, Matrix4 const& m) noexcept { return Matrix4(r / m[0], r / m[1], r / m[2], r / m[3]); }

		inline Matrix4& operator+=(Matrix4 const& r) noexcept { value[0] += r[0]; value[1] += r[1]; value[2] += r[2]; value[3] += r[3]; return *this; }
		inline Matrix4& operator-=(Matrix4 const& r) noexcept { value[0] -= r[0]; value[1] -= r[1]; value[2] -= r[2]; value[3] -= r[3]; return *this; }
		inline Matrix4& operator+=(T const r) noexcept { value[0] += r; value[1] += r; value[2] += r; value[3] += r; return *this; }
		inline Matrix4& operator-=(T const r) noexcept { value[0] -= r; value[1] -= r; value[2] -= r; value[3] -= r; return *this; }
		inline Matrix4& operator*=(Matrix4 const r) noexcept { *this = *this * r; return *this; }
		inline Matrix4& operator/=(Matrix4 const r) noexcept { *this *= r.inverse(); return *this; }
		inline Matrix4& operator*=(T const r) noexcept { value[0] *= r; value[1] *= r; value[2] *= r; value[3] *= r; return *this; }
		inline Matrix4& operator/=(T const r) noexcept { value[0] /= r; value[1] /= r; value[2] /= r; value[3] /= r; return *this; }

		inline bool operator==(Matrix4 const& r) const noexcept { return value[0] == r[0] && value[1] == r[1] && value[2] == r[2] && value[3] == r[3]; }
		inline bool operator!=(Matrix4 const& r) const noexcept { return !(*this == r); }

		inline vec& operator[](size_t const i) { return value[i]; }
		inline vec& operator[](size_t const i) const { return value[i]; }

		inline Matrix4 inverse() const noexcept
		{
			// adapted from GLM because i can't be assed to write this myself
			T c00 = value[2][2] * value[3][3] - value[3][2] * value[2][3];
			T c02 = value[1][2] * value[3][3] - value[3][2] * value[1][3];
			T c03 = value[1][2] * value[2][3] - value[2][2] * value[1][3];

			T c04 = value[2][1] * value[3][3] - value[3][1] * value[2][3];
			T c06 = value[1][1] * value[3][3] - value[3][1] * value[1][3];
			T c07 = value[1][1] * value[2][3] - value[2][1] * value[1][3];

			T c08 = value[2][1] * value[3][2] - value[3][1] * value[2][2];
			T c10 = value[1][1] * value[3][2] - value[3][1] * value[1][2];
			T c11 = value[1][1] * value[2][2] - value[2][1] * value[1][2];

			T c12 = value[2][0] * value[3][3] - value[3][0] * value[2][3];
			T c14 = value[1][0] * value[3][3] - value[3][0] * value[1][3];
			T c15 = value[1][0] * value[2][3] - value[2][0] * value[1][3];

			T c16 = value[2][0] * value[3][2] - value[3][0] * value[2][2];
			T c18 = value[1][0] * value[3][2] - value[3][0] * value[1][2];
			T c19 = value[1][0] * value[2][2] - value[2][0] * value[1][2];

			T c20 = value[2][0] * value[3][1] - value[3][0] * value[2][1];
			T c22 = value[1][0] * value[3][1] - value[3][0] * value[1][1];
			T c23 = value[1][0] * value[2][1] - value[2][0] * value[1][1];

			vec f0(c00, c00, c02, c03);
			vec f1(c04, c04, c06, c07);
			vec f2(c08, c08, c10, c11);
			vec f3(c12, c12, c14, c15);
			vec f4(c16, c16, c18, c19);
			vec f5(c20, c20, c22, c23);

			vec v0(value[1][0], value[0][0], value[0][0], value[0][0]);
			vec v1(value[1][1], value[0][1], value[0][1], value[0][1]);
			vec v2(value[1][2], value[0][2], value[0][2], value[0][2]);
			vec v3(value[1][3], value[0][3], value[0][3], value[0][3]);

			vec i0(v1 * f0 - v2 * f1 + v3 * f2);
			vec i1(v0 * f0 - v2 * f3 + v3 * f4);
			vec i2(v0 * f1 - v1 * f3 + v3 * f5);
			vec i3(v0 * f2 - v1 * f4 + v2 * f5);

			vec signA(+1, -1, +1, -1);
			vec signB(-1, +1, -1, +1);

			T inverse_determinant = 1 / determinant();

			return Matrix4(
				i0 * signA, i1 * signB, i2 * signA, i3 * signB
			) * inverse_determinant;
		}
		inline T determinant() const noexcept
		{
			T c0 = value[2][2] * value[3][3] - value[3][2] * value[2][3];
			T c1 = value[2][1] * value[3][3] - value[3][1] * value[2][3];
			T c2 = value[2][1] * value[3][2] - value[3][1] * value[2][2];
			T c3 = value[2][0] * value[3][3] - value[3][0] * value[2][3];
			T c4 = value[2][0] * value[3][2] - value[3][0] * value[2][2];
			T c5 = value[2][0] * value[3][1] - value[3][0] * value[2][1];

			vec det_cof(
				+ (value[1][1] * c0 - value[1][2] * c1 + value[1][3] * c2),
				- (value[1][0] * c0 - value[1][2] * c3 + value[1][3] * c4),
				+ (value[1][0] * c1 - value[1][1] * c3 + value[1][3] * c5),
				- (value[1][0] * c2 - value[1][1] * c4 + value[1][2] * c5)
			);

			return
				value[0][0] * det_cof[0] + value[0][1] * det_cof[1] +
				value[0][2] * det_cof[2] + value[0][3] * det_cof[3];
		}
		inline Matrix4 transpose() const noexcept
		{
			return Matrix4(
				value[0][0],
				value[1][0],
				value[2][0],
				value[3][0],
				value[0][1],
				value[1][1],
				value[2][1],
				value[3][1],
				value[0][2],
				value[1][2],
				value[2][2],
				value[3][2],
				value[0][3],
				value[1][3],
				value[2][3],
				value[3][3]
			);
		}
	};

	using Matrix4I = Matrix4<int32_t>;
	using Matrix4U = Matrix4<uint32_t>;
	using Matrix4F = Matrix4<float>;

	// 颜色（有黑魔法）

	struct alignas(uint32_t) Color4B
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;

		Color4B() : r(0), g(0), b(0), a(0) {}
		Color4B(uint32_t ARGB) : r(ARGB >> 16 & 255), g(ARGB >> 8 & 255), b(ARGB & 255), a(ARGB >> 24 & 255) {}
		Color4B(uint8_t const r_, uint8_t const g_, uint8_t const b_) : r(r_), g(g_), b(b_), a(static_cast<uint8_t>(255u)) {}
		Color4B(uint8_t const r_, uint8_t const g_, uint8_t const b_, uint8_t const a_) : r(r_), g(g_), b(b_), a(a_) {}

		inline void color(uint32_t ABGR) noexcept { *((uint32_t*)(&r)) = ABGR; }
		inline uint32_t color() const noexcept { return *((uint32_t*)(&r)); }

		bool operator==(Color4B const& right) const noexcept
		{
			return color() == right.color();
		}
		bool operator!=(Color4B const& right) const noexcept
		{
			return color() != right.color();
		}
	};

	// 分数

	struct Rational
	{
		uint32_t numerator; // 分子
		uint32_t denominator; // 分母

		Rational() : numerator(0), denominator(0) {}
		Rational(uint32_t const numerator_) : numerator(numerator_), denominator(1) {}
		Rational(uint32_t const numerator_, uint32_t const denominator_) : numerator(numerator_), denominator(denominator_) {}
	};

	// 字符串

	using StringView = std::string_view;

	// 引用计数

	struct IObject
	{
		virtual intptr_t retain() = 0;
		virtual intptr_t release() = 0;
		virtual ~IObject() {};
	};
	
	template<typename T = IObject>
	class ScopeObject
	{
	private:
		T* ptr_;
	private:
		inline void internal_retain() { if (ptr_) ptr_->retain(); }
		inline void internal_release() { if (ptr_) ptr_->release(); ptr_ = nullptr; }
	public:
		T* operator->() { return ptr_; }
		T* operator*() { return ptr_; }
		T** operator~() { internal_release(); return &ptr_; }
		ScopeObject& operator=(std::nullptr_t) { internal_release(); return *this; }
		ScopeObject& operator=(T* ptr) { if (ptr_ != ptr) { internal_release(); ptr_ = ptr; internal_retain(); } return *this; }
		ScopeObject& operator=(ScopeObject& right) { if (ptr_ != right.ptr_) { internal_release(); ptr_ = right.ptr_; internal_retain(); } return *this; }
		ScopeObject& operator=(ScopeObject const& right) { if (ptr_ != right.ptr_) { internal_release(); ptr_ = right.ptr_; internal_retain(); } return *this; }
		operator bool() { return ptr_ != nullptr; }
		ScopeObject& attach(T* ptr) { internal_release(); ptr_ = ptr; return *this; }
		T* detach()  { T* tmp_ = ptr_; ptr_ = nullptr; return tmp_; }
		ScopeObject& reset() { internal_release(); return *this; }
		T* get() const { return ptr_; }
	public:
		ScopeObject() : ptr_(nullptr) {}
		ScopeObject(T* ptr) : ptr_(ptr) { internal_retain(); }
		ScopeObject(ScopeObject& right) : ptr_(right.ptr_) { internal_retain(); }
		ScopeObject(ScopeObject const& right) : ptr_(right.ptr_) { internal_retain(); }
		ScopeObject(ScopeObject&& right) noexcept : ptr_(right.ptr_) { right.ptr_ = nullptr; }
		ScopeObject(ScopeObject const&&) = delete;
		~ScopeObject() { internal_release(); }
	};

	struct IData : public IObject
	{
		virtual void* data() = 0;
		virtual size_t size() = 0;

		static bool create(size_t size, IData** pp_data);
		static bool create(size_t size, size_t align, IData** pp_data);
	};
}
