# str_test_socket
packet words Initials test , use tcp client and tcp server

在此目录下进行 make all 或make clean 即可编译或去编译。编译产生的可执行文件在_bin目录(client_demo server_demo)

2.测试题目采用tcp进行进程间通信。_bin/client_demo为tcp客户端，_bin/server_demo为tcp服务端,为方便测试 限制了最大客户端连接数量为20个 宏名为 MAX_TCP_SERVER_ACCEPT_CNT
	限制了用户输入字符串的长度为1000，超出会截断分两次发送。
	
3.测试方法	1 将代码导入Ubuntu空间，在Ubuntu虚拟机里面建立一个终端窗口，切到_bin目录，先运行服务端 ./server_demo 会使用默认端口19003建立监听。如要修改端口号为19004或其他，可输入 ./server_demo 19004 运行服务端
			2.新建立一个终端窗口，切到_bin目录，运行客户端 ./client_demo 会使用默认端口19003和环回地址127.0.0.1尝试连接服务端，若服务端没有先运行，客户端连接失败将退出程序。如要修改IP或端口号为19004或其他，
				可输入 ./client_demo 192.168.1.1 19004 运行客户端 或 ./client_demo 192.168.1.2 运行客户端

	

5 时间有限，代码注释不够全面。暂时没有使用unity单元测试工具，如有需求，可后面完善。

	客户端 服务端采用自定义的通信协议格式进行通信。有帧头 校验码 错误码 应答位等控制。暂不展开描述，协议细节有兴趣可进一步沟通
	
	代码目录结构为
	
	_bin 编译后可执行文件目录
	apptool 		存放通用文件工具代码
	client_demo		存放客户端进程代码
	server_demo		存放服务端进程代码
	client_demo/_obj	存放编译过程文件
	server_demo/_obj	存放编译过程文件
	
	
	主要功能函数描述：
	
	glo_type.h 用户保存全局定义   NDEBUG 宏用于控制调试信息和断言的开启关闭
	GetWordsFromStr()  函数用于解析出输入字符串的所有单词，有最大单词数的输出限制，单词有最大长度限制
	PackWordsInitials()   用于将单词数组的首字母解析出来并转成大写，并组装成字符串输出
	socketapi.c 用于tcp的接口封装。TcpServSockConnect()函数用于建立服务端（默认端口19003） TcpAcceptCliConnection() TcpCheckMultiCliData() 函数用于监听客户端数据
									SocketTryConnect() 函数用于客户端连接服务端的IP和端口。
	GetStdinStr()用户获取用户键盘输入的字符串，回车后发送。TcpDataPackWithProp()函数用于将字符串封装成协议数据包进行tcp链路发送
