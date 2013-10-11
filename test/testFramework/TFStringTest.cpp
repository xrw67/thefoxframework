#include "TFStringTest.h"
#include "../../base/String.h"

CPPUNIT_TEST_SUITE_REGISTRATION(TFStringTest);

//CPPUNIT_ASSERT_DOUBLES_EQUAL (1.0, 1.1, 0.05);
//CPPUNIT_ASSERT_EQUAL (12, 12);
//CPPUNIT_ASSERT (1 == 0);

void TFStringTest::testGetLength()
{
	thefox::String s("asdf123456   ~g \r\n\t ");
	
	CPPUNIT_ASSERT_EQUAL(strlen("asdf123456   ~g \r\n\t "),(size_t)s.length());
}

void TFStringTest::testReserve()
{
	thefox::String s(" qwertyuiop[]");

	CPPUNIT_ASSERT(s.reverse() == "][poiuytrewq ");
}