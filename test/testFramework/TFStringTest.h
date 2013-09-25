#pragma once
#include <cppunit/extensions/HelperMacros.h>

class TFStringTest : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TFStringTest );
	CPPUNIT_TEST(testGetLength);
	CPPUNIT_TEST_SUITE_END();

protected:
	void testGetLength();
};

