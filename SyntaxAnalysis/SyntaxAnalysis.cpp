#include"SyntaxStruct.h"


int main()
{
	//SmartString Test
	/*SmartString smartstring;
	smartstring.init(10);
	smartstring.append('w');
	smartstring.append('d');
	smartstring.append('3');
	smartstring.print();
	printf("\n%d\n", smartstring.size());
	printf("%d\n", smartstring.capacity());
	system("pause");
	smartstring.reinit(15);
	smartstring.append('s');
	smartstring.append('d');
	smartstring.print();
	printf("\n%d\n", smartstring.size());
	printf("%d\n", smartstring.capacity());
	system("pause");
	smartstring.reset();
	smartstring.append('1');
	smartstring.append('2');
	smartstring.print();
	printf("\n%d\n", smartstring.size());
	printf("%d\n", smartstring.capacity());
	system("pause");
	smartstring.free();
	printf("\n%d\n", smartstring.size());
	printf("%d\n", smartstring.capacity());
	system("pause");*/

	//SmartArray Test
	SmartArray smartarray;
	smartarray.init(20);
	smartarray.add("MMP");
	smartarray.add("NMB");
	smartarray.add("MLGB");
	printf("%d\n",smartarray.get_count());
	printf("%d\n", smartarray.size());
	printf("%d\n", smartarray.capacity());
	smartarray.print();
	system("pause");
	smartarray.reinit(50);
	smartarray.add("WQNMLGB");
	printf("%d\n", smartarray.get_count());
	printf("%d\n", smartarray.size());
	printf("%d\n", smartarray.capacity());
	smartarray.print();
	system("pause");
	printf("%d\n", smartarray.research('N'));
	system("pause");

	smartarray.free();
	printf("%d\n", smartarray.get_count());
	printf("%d\n", smartarray.size());
	printf("%d\n", smartarray.capacity());
	smartarray.print();
	system("pause");

}