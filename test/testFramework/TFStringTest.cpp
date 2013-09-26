#include "TFStringTest.h"
#include "../../common/tfstring.h"

CPPUNIT_TEST_SUITE_REGISTRATION(TFStringTest);

//CPPUNIT_ASSERT_DOUBLES_EQUAL (1.0, 1.1, 0.05);
//CPPUNIT_ASSERT_EQUAL (12, 12);
//CPPUNIT_ASSERT (1 == 0);

void TFStringTest::testGetLength()
{
	TFString s("asdf123456   ~g \r\n\t ");
	
	CPPUNIT_ASSERT_EQUAL(strlen("asdf123456   ~g \r\n\t "),(size_t)s.Length());
}

void TFStringTest::testReserve()
{
	TFString s(" qwertyuiop[]");
	
	CPPUNIT_ASSERT(s.Reverse() == "][poiuytrewq ");
	CPPUNIT_ASSERT(s.Reverse() == " ][poiuytrewq");
}