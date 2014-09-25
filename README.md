# TheFox Cpp Framework

## 简介 ##
一个C++的基础开发库，包含包括常见的C++常用工具类（file、mutex、semaphore、thread、线程池等）、mysql封装、net-snmp封装、net库、rpc、log库。

现在主要工作平台是Windows，最近也在将其中的一部分慢慢的实现Windows和Linux跨平台

本库尽量保证轻量级，功能比较简单

**请保证您的编译器支持C++11**

## 文件说明 ##

		|thefoxframework
		--|base 比较基础的功能封装
		  --|common.h 公用的一些宏定义和类型定义	
		  --|debug_win.h Windows下帮助调试的工具
		  --|file_tools.h 文件操作工具
		  --|ini_file.h ini文件的操作的封装
		  --|ip_addr_utill.h 提供Ip地址字符串类型和整型之间互换的函数
		  --|mailslot.h Windows下邮槽的封装
		  --|mem_pool.h 内存池的封装
		  --|object_pool.h 对象池的封装
		  --|ring_buffer.h 环形缓冲的封装
		  --|observer.h Observer模式的封装
		  --|singleton.h 对Singleton模式的封装
		  --|string_util.h 提供一些字符串操作函数
		  --|time_util.h 时间戳帮助函数
		  --|timestamp.h 精确到微妙的时间戳类
		  --|uuid.h 产生UUID字符串的函数
		  [多线程开发工具]
		  --|atomic_integer.h 对原子操作的封装 
		  --|condition.h 条件变量的封装
		  --|event.h Windows下Event的封装
		  --|mutex.h 互斥锁的封装
		  --|semaphore.h 对信号量的封装
		  --|read_write_lock.h 读写锁
		  --|thread.h 对线程操作的封装
		  --|thread_pool.h 一个线程池的封装
		--|log 一个日志库
		  --|logging.h{.cc} 操作日志字符串，日志库的主要逻辑代码都在这里
		  --|log_file.h(.cc) 把日志字符串写到文件的类
		  --|log_stdout.h{.cc} 把日志字符串输出到STDOUT上
		  --|log_syslog.h{.cc} 把日志字符串通过syslog协议输出
		--|net 一个网络开发框架
		  --|acceptor.h{.cc} 监听TCP端口的类
		  --|buffer.h 应用层缓存类（接收和发送数据用）
		  --|callback.h 网络框架提供给上层的回调函数
		  --|event.h {.cc} 表示一个异步IO事件
		  --|event_loop.h{.cc} 提供异步IO时间循环的支持
		  --|epoll_event.h{.cc} EPOLL封装的代码
		  --|iocp_event.h{.cc} IOCP封装的代码
		  --|inet_address.h Socket地址的封装
		  --|socket.h{.cc} Socket操作的封装
		  --|tcp_connection.h{.cc} 一个tcp会话的封装 ，这是网络库的核心
		  --|tcp_client.h{.cc} tcp客户端操作的封装
		  --|tcp_server.h{.cc} tcp服务器操作的封装
		--|db mysql数据库连接的封装
		  --|mysql_connection.h mysql连接的封装
		  --|mysql_result_set.h 查询结果集的封装
		  --|mysql_util.h 经常使用的sql操作的封装
		--|snmp SNMP客户端操作类
		--|rpc 基于net+protobuf的rpc实现
		--|3rd 第三方库
		  --|aes aes加密算法库
		  --|protobuf-2.5.0 protobuf库
		  --|rapidjson json格式操作库
		  --|rapidxml xml格式操作库
		  --|base64.hpp base64操作类
		  --|md5.hpp md5操作类
		  --|sha1.hpp sha1操作类
		  --|xml2json.h xml格式和json格式互相转换的类


## 项目地址 ##
- GitHub： https://github.com/macwe/thefoxframework

- Git@OSC: http://git.oschina.net/macwe/thefoxframework

## 联系我 ##
欢迎提意见，大家一起交流

交流QQ群: 303435748

E-mail: macwe1024@gmail.com