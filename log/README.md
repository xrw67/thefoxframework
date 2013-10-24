日志操作库
===============

使用：
#include <log/log.h>

/*
* @filename log.h
* @brief log writer library
* @author macwe@qq.com
* example:
* {
*   // step1:
*   thefox::initLog("basename");
*   // step2:
*   LOG_TRACE << "log text"<< 123 << ' ' << -3.14;
*   LOG_DEBUG << "log text"<< 123 << ' ' << -3.14;
*   LOG_INFO << "log text"<< 123 << ' ' << -3.14;
*   LOG_WARN << "log text"<< 123 << ' ' << -3.14;
*   LOG_ERROR << "log text"<< 123 << ' ' << -3.14;
*   LOG_FATAL << "log text"<< 123 << ' ' << -3.14;
* }
*/