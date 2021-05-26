#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "Math/Vector2.h"
#include "vec3f.h"
#include "Vec4f.h"
#include "Mat4x4.h"
#include "../RayTracer/shapes.h"
#include "vector.h"
#include <vector>


TEST_CASE("Testing Vector3 functionality")
{
	SUBCASE("Construction")
	{
		Vec3f test{};
		CHECK(test.m_x == 0.f);
		CHECK(test.m_y == 0.f);
		CHECK(test.m_z == 0.f);

		Vec3f test2{ 1.f,0,0 };
		CHECK(test2.m_x == 1.f);
		CHECK(test2.m_y == 0.f);
		CHECK(test2.m_z == 0.f);

		Vec3f test3{ 1.f,2.f,0 };
		CHECK(test3.m_x == 1.f);
		CHECK(test3.m_y == 2.f);
		CHECK(test3.m_z == 0.f);

		Vec3f test4{ 1.f,2.f,3.f };
		CHECK(test4.m_x == 1.f);
		CHECK(test4.m_y == 2.f);
		CHECK(test4.m_z == 3.f);
		/// The default constructor creates a zero vector.
		/// Creates a vector with the given components
		/// Zero all the components of the vector

		test4.clear();
		CHECK(test4.m_x == 0.f);
		CHECK(test4.m_y == 0.f);
		CHECK(test4.m_z == 0.f);
	}

	SUBCASE("Mathematical operators")
	{
		/// Adds the given vector to this
		Vec3f nullVec{ 0,0,0 };
		Vec3f b{ 1.f,2.f,3.f };
		Vec3f c{ 2.f,4.f,6.f };
		CHECK(nullVec + b == b);
		CHECK(nullVec + nullVec == nullVec);
		CHECK(b + b == c);
		/// Subtracts the given vector from this
		CHECK(nullVec - nullVec == nullVec);

		CHECK(b - b == nullVec);
		CHECK(nullVec - b == -1 * b);
		CHECK(c - b == b);
		/// Returns a copy of this vector scaled the given value
		CHECK(nullVec * nullVec == nullVec);
		CHECK(nullVec * b == nullVec);
		CHECK(c * nullVec == nullVec);
		CHECK(b * b == Vec3f{ 1.f,4.f,9.f });

		CHECK(3.f * nullVec == nullVec);
		CHECK(1.f * b == b);
		CHECK(nullVec * 3.f == nullVec);
		CHECK(b * 3.f == Vec3f{ 3.f, 6.f, 9.f });
		CHECK(c * 0.f == nullVec);

		/// Returns a copy of this vector scaled the inverse of the value
		CHECK(-3.f * nullVec == nullVec);
		CHECK(-1.f * b == Vec3f{ -1.f,-2.f,-3.f });
		CHECK(nullVec * -3.f == nullVec);
		CHECK(b * -3.f == Vec3f{ -3.f, -6.f, -9.f });
		CHECK(c * 0.f == nullVec);

		SUBCASE("Vector products")
		{
			/// Calculates and returns the dot product of this with the given vector
			CHECK(nullVec.dot(nullVec) == 0.f);
			CHECK(b.dot(nullVec) == 0.f);
			CHECK(b.dot(b) == 14.f);
			CHECK(b.dot(c) == 28.f);

			/// Calculates and returns the cross product of this vector with the given vector

			Vec3f d{ -3,0,7 };
			CHECK(nullVec.cross(nullVec) == nullVec);
			CHECK(b.cross(c) == nullVec);
			CHECK(b.cross(d) == Vec3f{ 14.f,-16.f,6.f });
		}

		SUBCASE("Length operations")
		{
			///  Gets the magnitude of this vector
			Vec3f d{ -3,0,7 };
			CHECK(nullVec.lenght() == 0.f);
			CHECK(b.lenght() == sqrtf(14));
			CHECK(c.lenght() == 2 * sqrtf(14));
			CHECK(d.lenght() == sqrtf(58));

			///  Gets the squared magnitude of this vector
			CHECK(nullVec.lenght() == 0.f);
			CHECK(b.lenght2() == 14);
			CHECK(c.lenght2() == 56);
			CHECK(d.lenght2() == 58);
			/// Returns the normalised version of a vector
			CHECK(b.getNormalCopy() == Vec3f{ 1.f / sqrtf(14.f), 2.f / sqrtf(14), 3.f / sqrtf(14) });
			CHECK(c.getNormalCopy() == Vec3f{ 2.f / sqrtf(56), 4.f / sqrtf(56), 6.f / sqrtf(56) });
			CHECK(d.getNormalCopy() == Vec3f{ -3.f / sqrtf(58), 0.f / sqrtf(58), 7.f / sqrtf(58) });

			/// Turns a non-zero vector into a vector of unit length
			CHECK(b.normalize() == Vec3f{ 1.f / sqrtf(14.f), 2.f / sqrtf(14), 3.f / sqrtf(14) });
			CHECK(c.normalize() == Vec3f{ 2.f / sqrtf(56), 4.f / sqrtf(56), 6.f / sqrtf(56) });
			CHECK(d.normalize() == Vec3f{ -3.f / sqrtf(58), 0.f / sqrtf(58), 7.f / sqrtf(58) });
		}
	}

	SUBCASE("Logical operations")
	{
		Vec3f nullVec{ 0,0,0 };
		Vec3f b{ 1.f,2.f,3.f };
		Vec3f c{ 2.f,4.f,6.f };
		/// Checks if the two vectors have identical components
		CHECK((nullVec == nullVec) == true);
		CHECK((nullVec == b) == false);
		CHECK((b == b) == true);
		CHECK((b == c) == false);
		/// Checks if the two vectors have non-identical components
		CHECK((nullVec != nullVec) == false);
		CHECK((nullVec != b) == true);
		CHECK((b != b) == false);
		CHECK((b != c) == true);
	}
}

TEST_CASE("Testing Vector4 functionality")
{
	SUBCASE("Construction")
	{
		Vec4f test{};
		CHECK(test[0] == 0.f);
		CHECK(test[1] == 0.f);
		CHECK(test[2] == 0.f);
		CHECK(test[3] == 0.f);

		Vec4f test2{ 1.f,0,0,0.f };
		CHECK(test2[0] == 1.f);
		CHECK(test2[1] == 0.f);
		CHECK(test2[2] == 0.f);
		CHECK(test2[3] == 0.f);

		Vec4f test3{ 1.f,2.f,0,0.f };
		CHECK(test3[0] == 1.f);
		CHECK(test3[1] == 2.f);
		CHECK(test3[2] == 0.f);
		CHECK(test3[3] == 0.f);

		Vec4f test4{ 1.f,2.f,3.f };
		CHECK(test4[0] == 1.f);
		CHECK(test4[1] == 2.f);
		CHECK(test4[2] == 3.f);
		CHECK(test4[3] == 0.f);

		Vec4f test5{ 1.f,2.f,3.f ,4.f };
		CHECK(test5[0] == 1.f);
		CHECK(test5[1] == 2.f);
		CHECK(test5[2] == 3.f);
		CHECK(test5[3] == 4.f);

		/// The deault constructor creates a zero vector.
		/// Creates a vector with the given components
		/// Zero all the components of the vector
	}

	SUBCASE("Mathematical operators")
	{
		/// Adds the given vector to this
		Vec4f nullVec{ 0,0,0,0.f };
		Vec4f b{ 1.f,2.f,3.f,4.f };
		Vec4f c{ 2.f,4.f,6.f,8.f };
		Vec4f test{ nullVec + b };
		CHECK(nullVec + b == b);
		CHECK(nullVec + nullVec == nullVec);
		CHECK(b + b == c);
		/// Subtracts the given vector from this
		CHECK(nullVec - nullVec == nullVec);
		CHECK(b - b == nullVec);
		CHECK(nullVec - b == -1 * b);
		CHECK(c - b == b);
		/// Returns a copy of this vector scaled the given value
		CHECK(nullVec * nullVec == nullVec);
		CHECK(nullVec * b == nullVec);
		CHECK(c * nullVec == nullVec);
		CHECK(b * b == Vec4f{ 1.f,4.f,9.f,16.f });

		CHECK(3.f * nullVec == nullVec);
		CHECK(1.f * b == b);
		CHECK(nullVec * 3.f == nullVec);
		CHECK(b * 3.f == Vec4f{ 3.f, 6.f, 9.f,12.f });
		CHECK(c * 0.f == nullVec);

		/// Returns a copy of this vector scaled the inverse of the value
		CHECK(-3.f * nullVec == nullVec);
		CHECK(-1.f * b == Vec4f{ -1.f,-2.f,-3.f,-4.f });
		CHECK(nullVec * -3.f == nullVec);
		CHECK(b * -3.f == Vec4f{ -3.f, -6.f, -9.f,-12.f });
		CHECK(c * 0.f == nullVec);

		SUBCASE("Vector products")
		{
			/// Calculates and returns the dot product of this with the given vector
			CHECK(nullVec.dot(nullVec) == 0.f);
			CHECK(b.dot(nullVec) == 0.f);
			CHECK(b.dot(b) == 30.f);
			CHECK(b.dot(c) == 60.f);
		}

		SUBCASE("Length operations")
		{
			///  Gets the magnitude of this vector
			Vec4f d{ -3.f,0.f,7.f,3.f };
			CHECK(nullVec.lenght() == 0.f);
			CHECK(b.lenght() == sqrtf(30));
			CHECK(c.lenght() == sqrtf(120));
			CHECK(d.lenght() == sqrtf(67));

			///  Gets the squared magnitude of this vector
			CHECK(nullVec.lenght() == 0.f);
			CHECK(b.lenght2() == 30);
			CHECK(c.lenght2() == 120);
			CHECK(d.lenght2() == 67);
			/// Returns the normalised version of a vector

			/// Turns a non-zero vector into a vector of unit length
			CHECK(b.normalize() == Vec4f{ 1.f / sqrtf(30.f), 2.f / sqrtf(30), 3.f / sqrtf(30),4.f / sqrtf(30) });
			CHECK(c.normalize() == Vec4f{ 2.f / sqrtf(120), 4.f / sqrtf(120), 6.f / sqrtf(120),8.f / sqrtf(120) });
			CHECK(d.normalize() == Vec4f{ -3.f / sqrtf(67), 0.f / sqrtf(67), 7.f / sqrtf(67),3.f / sqrtf(67) });
		}
	}

	SUBCASE("Logical operations")
	{
		Vec3f nullVec{ 0,0,0 };
		Vec3f b{ 1.f,2.f,3.f };
		Vec3f c{ 2.f,4.f,6.f };
		/// Checks if the two vectors have identical components
		CHECK((nullVec == nullVec) == true);
		CHECK((nullVec == b) == false);
		CHECK((b == b) == true);
		CHECK((b == c) == false);
		/// Checks if the two vectors have non-identical components
		CHECK((nullVec != nullVec) == false);
		CHECK((nullVec != b) == true);
		CHECK((b != b) == false);
		CHECK((b != c) == true);
	}
}

TEST_CASE("Testing Matrix33 functionality")
{
	SUBCASE("Construction")
	{
		/// Construct a new matrix from explicit values
		/// Construct a new identity matrix

		/// Creates an identity matrix
		/// Creates a translation matrix
		/// Creates a scale matrix
		/// Creates a uniform scale matrix

		/// Creates a rotation matrix around an arbitrary axis (angle in radians)
		/// Creates a rotation matrix around the x axis (angle in radians)
		/// Creates a rotation matrix around the y axis (angle in radians)
		/// Creates a rotation matrix around the z axis (angle in radians)
	}

	SUBCASE("Mathematical operations")
	{
		/// Matrix addition
		/// Matrix subtraction
		/// Matrix multiplication

		SUBCASE("Inversion")
		{
			/// Get the determinant of this matrix
			/// Inverts this matrix
			/// Transposes this matrix
		}
	}

	/// Get the x axis
	/// Get the y axis
	/// Get the z axis
}

TEST_CASE("Testing Matrix44 functionality")
{
	SUBCASE("Construction")
	{
		// Matrix construction using values.
		Mat4x4 test
		{
			Vec4f{1.f,2.f,3.f,4.f},
			Vec4f{5.f,6.f,7.f,8.f},
			Vec4f{9.f,10.f,11.f,12.f},
			Vec4f{0.f,0.f,0.f,1.f}
		};

		CHECK(test.m_X == Vec4f{ 1.f,2.f,3.f,4.f });
		CHECK(test.m_Y == Vec4f{ 5.f,6.f,7.f,8.f });
		CHECK(test.m_Z == Vec4f{ 9.f,10.f,11.f,12.f });
		CHECK(test.m_W == Vec4f{ 0.f,0.f,0.f,1.f });
		/// Creates an identity matrix
		Mat4x4 identity{};
		CHECK(identity.m_X == Vec4f{ 1.f,0.f,0.f,0.f });
		CHECK(identity.m_Y == Vec4f{ 0.f,1.f,0.f,0.f });
		CHECK(identity.m_Z == Vec4f{ 0.f,0.f,1.f,0.f });
		CHECK(identity.m_W == Vec4f{ 0.f,0.f,0.f,1.f });

		// Check equality
		CHECK(test == test);
		CHECK(test != identity);

		Mat4x4 identityX2
		{
			Vec4f{2.f,0.f,0.f,0.f},
			Vec4f{0.f,2.f,0.f,0.f},
			Vec4f{0.f,0.f,2.f,0.f},
			Vec4f{0.f,0.f,0.f,1.f}
		};
		Mat4x4 identityX123
		{
			Vec4f{2.f,0.f,0.f,0.f},
			Vec4f{0.f,4.f,0.f,0.f},
			Vec4f{0.f,0.f,6.f,0.f},
			Vec4f{0.f,0.f,0.f,1.f}
		};

		Mat4x4 scaleTest{ identity };
		/// Creates a uniform scale matrix
		CHECK(scaleTest.scaleUniform(2.f) == identityX2);
		/// Creates a scale matrix
		CHECK(scaleTest.scale(Vec3f{ 1.f,2.f,3.f }) == identityX123);

		/// Creates a rotation matrix around the x axis (angle in radians)
		Mat4x4 rotXTest{ identity };
		Mat4x4 rotXVerify
		{
			Vec4f{1,	                      0	 ,                     0,	0},
			Vec4f{0,	35355339073.f / 50000000000.f, -7071067809.f / 10000000000.f,	0},
			Vec4f{0,	 7071067809.f / 10000000000.f,	35355339073.f / 50000000000.f,	0},
			Vec4f{0,	                      0	       ,               0,	1},
		};
		rotXTest.rotateX(45);
		CHECK(rotXTest == rotXVerify);
		/// Creates a rotation matrix around the y axis (angle in radians)
		Mat4x4 rotYTest{ identity };
		Mat4x4 rotYVerify{
			Vec4f{35355339073.f / 50000000000.f,0,7071067809.f / 10000000000.f,0},
			Vec4f{0,1,0,0},
			Vec4f{-7071067809.f / 10000000000.f,0,35355339073.f / 50000000000.f,0},
			Vec4f{0,0,0,1}
		};
		rotYTest.rotateY(45);
		CHECK(rotYTest == rotYVerify);
		/// Creates a rotation matrix around the z axis (angle in radians)
		Mat4x4 rotZTest{ identity };
		Mat4x4 rotZVerify{
			Vec4f{35355339073.f / 50000000000.f,-7071067809.f / 10000000000.f,0.f ,0},
			Vec4f{7071067809.f / 10000000000.f,35355339073.f / 50000000000.f,0,0},
			Vec4f{0,0,1.f,0},
			Vec4f{0,0,0,1}
		};
		rotZTest.rotateZ(45);
		CHECK(rotZTest == rotZVerify);

		/// Creates a perspective projection matrix from camera settings
		/// Creates a look at matrix, usually a view matrix
	}

	SUBCASE("Mathematical operations")
	{
		Mat4x4 identity{};
		/// Matrix addition
		Mat4x4 additionCheck
		{
			Vec4f{2.f,0.f,0.f,0.f},
			Vec4f{0.f,2.f,0.f,0.f},
			Vec4f{0.f,0.f,2.f,0.f},
			Vec4f{0.f,0.f,0.f,2.f}
		};
	
		CHECK(identity + identity == additionCheck);
		Mat4x4 addEquals{ identity };
		addEquals += addEquals;
		CHECK(addEquals == additionCheck);
		
		/// Matrix subtraction
		Mat4x4 substractionTestCheck
		{
			Vec4f{0.f,0.f,0.f,0.f},
			Vec4f{0.f,0.f,0.f,0.f},
			Vec4f{0.f,0.f,0.f,0.f},
			Vec4f{0.f,0.f,0.f,0.f}
		};
		CHECK(identity - identity == substractionTestCheck);
		Mat4x4 minusEquals{ identity };
		minusEquals -= minusEquals;
		CHECK(minusEquals == substractionTestCheck);
		/// Matrix multiplication
		///
		CHECK(identity * identity == identity);
		Mat4x4 a{ Vec4f{1,2,3,4},{5,6,7,8},{9,10,11,12},{0,0,0,1} };
		Mat4x4 b{ Vec4f{12,11,10,9},{8,7,6,5},{4,3,2,1},{0,0,0,1} };
		Mat4x4 c{ Vec4f{40,34,28,26},{136,118,100,90},{232,202,172,154},{0,0,0,1} };

		CHECK(a * b == c);
		SUBCASE("Inversion")
		{
			Mat4x4 identityInverseTest{ identity };
			identityInverseTest.inverse();
			CHECK(identityInverseTest == identity);
			Mat4x4 scale2{ Vec4f{2,0,0,2},Vec4f{0,2,0,2},Vec4f{0,0,2,2},Vec4f{0,0,0,1} };
			Mat4x4 scaleInverseVerify{ Vec4f{1.f / 2.f,0,0,-1},Vec4f{0,1.f / 2.f,0,-1},Vec4f{0,0,1.f / 2.f,-1},Vec4f{0,0,0,1} };
			scale2.inverse();
			CHECK( scale2 == scaleInverseVerify);
			
		}
	}

	SUBCASE("Transformation")
	{
		Mat4x4 scale2Mat
		{
			Vec4f{2.f,0.f,0.f,2.f},
			Vec4f{0.f,2.f,0.f,3.f},
			Vec4f{0.f,0.f,2.f,4.f},
			Vec4f{0.f,0.f,0.f,1.f}
		};
		/// Transform the direction vector of this matrix
		Vec4f testWithoutTransform{ 5.f,3.f,7.f,0.f };
		Vec4f testWithoutTransformVerify{ 10.f,6.f,14.f,0.f };
		CHECK(scale2Mat * testWithoutTransform == testWithoutTransformVerify);
		/// Transform the given vector by this matrix.
		Vec4f testWithTransform{ 5.f,3.f,7.f,1.f };
		Vec4f testWithTransformVerify{ 12.f,9.f,18.f,1.f };
		CHECK(scale2Mat * testWithTransform == testWithTransformVerify);
		
	
	}
	Mat4x4 scale2Mat
	{
		Vec4f{2.f,0.f,0.f,2.f},
		Vec4f{0.f,2.f,0.f,3.f},
		Vec4f{0.f,0.f,2.f,4.f},
		Vec4f{0.f,0.f,0.f,1.f}
	};
	/// Retrieve translation part of the matrix
	Vec3f getTranslationVerify{ 2.f,3.f,4.f };
	CHECK(scale2Mat.getTranslation() == getTranslationVerify);
	/// Set the translation of the matrix
	Mat4x4 identity{};
	identity.setTranslation(getTranslationVerify);
	CHECK(identity.getTranslation() == getTranslationVerify);

	/// Get the x orientation axis
	/// Get the y orientation axis
	/// Get the z orientation axis

	/// Sets the orientation of the matrix to the orthogonal basis vector
}

TEST_CASE("Testing Intersections")
{
	SUBCASE("Sphere")
	{
		Sphere testSphere{ {0,0,0},1.f,{},{} };
		Ray centerHit{ {0.f,0.f,-1.f},{0.f,0.f,1.f} };
		Ray edgeHit{ {0.f,0.f,-1.f},{1.f,0.f,1.f} };
		Ray behind{ {0.f,0.f,-1.f},{0.f,0.f,-2.f} };
		Ray completeMiss{ {0.f,2.f,0.f},{0.f,0.f,2.f} };
		Vec3f tempHit{};
		Shape* tempShape{};

		CHECK(testSphere.IntersectionTest(centerHit, tempHit, tempShape) == true);
		CHECK(testSphere.IntersectionTest(edgeHit, tempHit, tempShape) == true);
		CHECK(testSphere.IntersectionTest(behind, tempHit, tempShape) == false);
		CHECK(testSphere.IntersectionTest(completeMiss, tempHit, tempShape) == false);
	}
	SUBCASE("Plane")
	{
		Plane planeTest{ {0,0,0},{},{} };
		Ray parralel{ {0.f,0.f,-1.f},{0.f,1.f,1.f} };
		Ray hit{ {0.f,-1.f,-1.f},{0.f,1.f,1.f} };
		Ray behind{ {0.f,1.f,0.f},{0.f,1.f,-2.f} };
	
		Vec3f tempHit{};
		Shape* tempShape{};
		CHECK(planeTest.IntersectionTest(behind, tempHit, tempShape) == false);
		CHECK(planeTest.IntersectionTest(parralel, tempHit, tempShape) == false);
		CHECK(planeTest.IntersectionTest(hit, tempHit, tempShape) == true);
		
	
	}
	SUBCASE("Cube")
	{
		OBox testBox({ -0.5f,-0.5f,0.5f }, { 0.5f,0.5f,-0.5f }, { }, {});
		Ray centerHit{ {0.f,0.f,-1.f},{0.f,0.f,1.f} };
		Ray edgeHit{ {0.f,0.f,-1.f},{0.5f,0.f,1.f} };
		Ray behind{ {0.f,0.f,-1.f},{0.f,0.f,-2.f} };
		Ray completeMiss{ {0.f,2.f,0.f},{0.f,0.f,2.f} };
		Vec3f tempHit{};
		Shape* tempShape{};

		CHECK(testBox.IntersectionTest(centerHit, tempHit, tempShape) == true);
		CHECK(testBox.IntersectionTest(edgeHit, tempHit, tempShape) == true);
		CHECK(testBox.IntersectionTest(behind, tempHit, tempShape) == false);
		CHECK(testBox.IntersectionTest(completeMiss, tempHit, tempShape) == false);
	}
	
}

TEST_CASE("Testing std::vector replacement")
{

	SUBCASE("Initialization")
	{
		ldk::Vector<int> lVecInitTest1{1,2,3,4,5,6};
		std::vector<int> stdVecInitTest1{ 1,2,3,4,5,6 };

		for (int i = 0; i < stdVecInitTest1.size(); ++i)
		{
			CHECK(lVecInitTest1[i] == stdVecInitTest1[i]);
		}
		ldk::Vector<int> lVecInitTest2;
		lVecInitTest2.resize(2);
		std::vector<int> stdVecInitTest2;
		stdVecInitTest2.resize(2);
		lVecInitTest2[0] = 3;
		stdVecInitTest2[0] = 3;
		CHECK(lVecInitTest2[0] == stdVecInitTest2[0]);
		
		
		
	}
	SUBCASE("vector functions")
	{
		ldk::Vector<int> ldkVectorPushbackTest;
		std::vector<int> stdVectorPushbackTest;
		// Checking pushback
		for (int i = 0; i < 1000; ++i)
		{
			int random{ rand() };
			ldkVectorPushbackTest.push_back(random);
			stdVectorPushbackTest.push_back(random);
			CHECK(ldkVectorPushbackTest[i] == stdVectorPushbackTest[i]);
			
		}
		// checking begin and end
		CHECK(ldkVectorPushbackTest.front() == stdVectorPushbackTest.front());
		CHECK(ldkVectorPushbackTest.back() == stdVectorPushbackTest.back());

		// Checking empty
		ldk::Vector<int> ldkVectorEmptyTest;
		std::vector<int> stdVectorEmptyTest;
		CHECK(ldkVectorPushbackTest.empty() == stdVectorPushbackTest.empty());
		CHECK(ldkVectorEmptyTest.empty() == stdVectorEmptyTest.empty());

		// Checking empty
		ldk::Vector<int> ldkVectorResizeTest;
		std::vector<int> stdVectorResizeTest;
		ldkVectorResizeTest.resize(100);
		stdVectorResizeTest.resize(100);

		for (int i = 0; i < 100; ++i)
		{
			CHECK(ldkVectorResizeTest[i] == stdVectorResizeTest[i]);
		}
		
	}
}