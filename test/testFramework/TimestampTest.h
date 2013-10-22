#pragma once
#include <cppunit/extensions/HelperMacros.h>

class TimestampTest : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE(TFStringTest);
	CPPUNIT_TEST(testConstruct);
	CPPUNIT_TEST_SUITE_END();
protected:
	void testConstructor();
};

