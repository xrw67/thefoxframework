#include "TestApp.h"
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/mfc/TestRunner.h> 

CTestApp::CTestApp(void)
{
}


CTestApp::~CTestApp(void)
{
}

BOOL CTestApp::InitInstance()
{

    CppUnit::MfcUi::TestRunner runner;
    CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
    runner.addTest(registry.makeTest());
    runner.run();
    return FALSE;
} 

CTestApp theApp;