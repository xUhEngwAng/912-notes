## exception

+ 基本结构`try-catch`

	try{
		expression	
	}catch(errorType err){
		expression
	}catch(errorType err){
		expression
	}...
+ 文件支持
	
	#include <stdexcept>
	//#include <exception>
	using std::runtime_error
+ 具体使用方法
	- try block内放可能会产生异常的语句，并抛出(throw)相应的异常

	try{
		if(...) throw runtime_error("fatal error: dividing zero");
		if(...) throw ...
		//else
		normal expression
	}
	- 在catch中检查可能会出现的异常，若异常与catch的异常类型对应，则进入相应的异常处理程序

	try{
		...
	}catch(runtime_error err){
		cout << err.what() << endl;//err.what()返回抛出异常时初始化的字符串
		//other handling
		cout << "continue or not(y/n): ";
		char c;
		cin >> c;
		if(c == 'n' || c == 'N') break;
		//else continue
	} 