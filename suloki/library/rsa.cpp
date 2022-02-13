#include"rsa.h"
/*
//核心算法#######################################
//核心算法#######################################
RSA::RSA()
{
	ProduceKeys();
}
//加密与解密单个信息
//这里只是密钥不同，函数则是相同的
int8192_t RSA::ecrept(int8192_t msg, int8192_t key, int8192_t pkey)
{
	//加密公式为：ret = 信息^key%n

	//DO优化，快速幂取模

	//最终的结果为：ret = 信息^key%n == (A0 * A1 * A2 * …… * An)

	int8192_t retmsg = 1;

	int8192_t a = msg;

	int8192_t c = pkey;

	while (key)

	{

		if (key & 1)//从第一个不是0的位开始乘

		{

			//A0 == a----> A0 == a^2^0 %n ----> a%n%n == a% n

			//A1 == A0 * A0 % n

			//第1个信息位a%n

			//第二个为 A0 * A1

			//第n个则是 (A0 * A1 * A2 * …… *A(n - 1) *  An)

			//即就是 (A0 * A1 * A2 * …… *A(n - 1) *   A(n - 1) * A(n - 1) )

			//9当前结果

			retmsg = (retmsg * a) % c;

		}
		//????? 曾经发生异常
		//下一次
		key >>= 1;
		a = (a * a) % c;
	}
	return retmsg;
}

int8192_t RSA::ProducePrime()
{

	br::mt19937 ran(time(nullptr));

	//br::uniform_int_distribution<bm::int1024_t> dist(1, bm::int1024_t(1) << 50);// 随机大数
	br::uniform_int_distribution<int8192_t> dist(1, int8192_t(1) << 256);// 随机大数

	while (1)

	{

		int8192_t tmp = dist(ran);

		if (is_prime(tmp))

		{

			return tmp;

		}

	}

	return 0;
}

bool RSA::is_prime(int8192_t prime)
{
	//std::cout << "is prime" << std::endl;

	br::mt11213b gen(time(nullptr));

	if (miller_rabin_test(prime, 25, gen))
	{

		if (miller_rabin_test((prime - 1) / 2, 25, gen))
		{

			return true;

		}

	}

	return false;
}

void RSA::ProduceKeys()
{
	//?????
	//
	//std::ifstream inputfile("./CONFIG.txt");

	//if (!inputfile.is_open())

	//{

	//	int8192_t prime1 = ProducePrime();

	//	int8192_t prime2 = ProducePrime();

	//	while (prime1 == prime2)

	//	{

		//	//Sleep(1000);

		//	prime2 = ProducePrime();

	//	}

	//	std::ofstream outputfile("./CONFIG.txt", std::ios::binary|std::ios::app);

	//	outputfile << prime1 << "\n";

	//	outputfile << prime2;



	//	outputfile.close();

	//}

	//std::string buff;

	//std::string tmp;

	//std::getline(inputfile, buff);

	//std::getline(inputfile, tmp);

	//int8192_t prime1(buff.c_str());

	//int8192_t prime2(tmp.c_str());

	//inputfile.close();
	//
	int8192_t prime1("22750156276775680151374566339008031821021984682173210963598012407657720645063");
	int8192_t prime2("19168473864007065693234791517284252634559880208658510295353593391443969842663");
	


	//std::cout << prime1 << std::endl;

	//std::cout << prime2 << std::endl;

	key_.nkey = ProduceNkey(prime1, prime2);



	int8192_t orla = ProduceOrla(prime1, prime2);

	key_.ekey = ProduceEkey(orla);



	key_.dkey = producedkey(key_.ekey, orla);
}

int8192_t RSA::ProduceNkey(int8192_t prime1, int8192_t prime2)
{
	//选择两个不相等的质数p，q，得 n = pq
	return prime1 * prime2;
}



int8192_t RSA::ProduceOrla(int8192_t prime1, int8192_t prime2)
{
	//欧拉公式，f(n)

	//如果a是制数：f(n) = n - 1

	//如果a, b都是制数：f(ab) = (a - 1)(b - 1)

	//如果a, b是互质：f(ab) = f(a)f(b)

	return (prime1 - 1)*(prime2 - 1);
}

int8192_t RSA::ProduceEkey(int8192_t orla)
{
	//得到f(n)，选择一个1 < E < f(n)，且E与n互质，得公钥(E, n)

	int8192_t i = 2;

	for (i; i < orla; ++i)

	{

		if (ProduceGcd(i, orla) == 1)

			break;

	}

	return i;
}



int8192_t RSA::ProduceGcd(int8192_t ekey, int8192_t orla)
{
	if (orla == 0)

		return ekey;



	return RSA::ProduceGcd(orla, ekey%orla);
}

int8192_t RSA::ProduceGcd(int8192_t ekey, int8192_t orla, int8192_t& x, int8192_t& y)
{
	//扩展的欧几里得算法

	if (orla == 0)

	{

		x = 1;

		y = 0;

		return ekey;

	}

	int8192_t ret = ProduceGcd(orla, ekey%orla, x, y);

	int8192_t tmpx = x;

	int8192_t tmpy = y;

	x = tmpy;

	y = tmpx - (ekey / orla)*tmpy;

	return ret;

	//if (orla == 0) 

	//	return ekey;

	//return RSA::ProduceGcd(orla, ekey%orla);
}

int8192_t RSA::producedkey(int8192_t ekey, int8192_t orla)
{
	//计算e与f(n)的逆元 D，得密钥(D, n)





	//由扩展的欧几里得算法



	int8192_t dkey = 1;

	int8192_t Y = 3;

	int8192_t tmp = ProduceGcd(ekey, orla, dkey, Y);

	dkey = (dkey%orla + orla) % orla;

	return dkey;
}



//加密过程############################
//加密过程############################
Key RSA::GetKey()
{
	return key_;

}

std::vector<int8192_t> RSA::Ecrept(std::string& str_in, int8192_t ekey, int8192_t pkey)
{
	std::vector<int8192_t> retv;

	for (size_t i = 0; i < str_in.size(); ++i)

	{

		retv.push_back(ecrept(str_in[i], ekey, pkey));

	}

	return retv;
}

std::string RSA::Decrept(std::vector<int8192_t>& ecrept_str, int8192_t dkey, int8192_t pkey)
{
	std::string retstr;

	for (size_t i = 0; i < ecrept_str.size(); ++i)

	{

		retstr += (char)ecrept(ecrept_str[i], dkey, pkey);

	}

	return retstr;
}

const int NUM = 256;//每次读256字节



void RSA::FileEcrept(const char* plain_file_in, const char* ecrept_file_out,
	int8192_t ekey, int8192_t pkey)
{

	std::ifstream file_in(plain_file_in, std::ios::binary);

	std::ofstream file_out(ecrept_file_out, std::ios::binary | std::ios::app);



	if (!(file_in.is_open() && file_out.is_open()))

	{

		std::cout << "open error" << std::endl;

		return;

	}

	char buff_in[NUM];

	int8192_t buff_out[NUM];

	int curnum;//当前读取的字节

	while (!file_in.eof())
	{

		file_in.read(buff_in, NUM);

		curnum = file_in.gcount();

		for (int i = 0; i < curnum; ++i)
		{

			buff_out[i] = ecrept((int8192_t)buff_in[i], ekey, pkey);

		}

		file_out.write((char*)buff_out, curnum * sizeof(int8192_t));

	}



	file_in.close();

	file_out.close();

}

void RSA::FileDecrept(const char* ecrept_file_in, const char* plain_file_out,
	int8192_t dkey, int8192_t pkey)
{

	std::ifstream file_in(ecrept_file_in, std::ios::binary);

	std::ofstream file_out(plain_file_out, std::ios::binary | std::ios::app);



	if (!(file_in.is_open() && file_out.is_open()))
	{

		std::cout << "open error" << std::endl;

		return;

	}

	int8192_t buff_in[NUM];

	char buff_out[NUM];

	int curnum;//当前读取的字节

	while (!file_in.eof())
	{

		file_in.read((char*)buff_in, NUM * sizeof(int8192_t));

		curnum = file_in.gcount();

		curnum /= sizeof(int8192_t);

		for (int i = 0; i < curnum; ++i)
		{

			buff_out[i] = (char)ecrept(buff_in[i], dkey, pkey);

		}

		file_out.write(buff_out, curnum);

	}



	file_in.close();

	file_out.close();

}

void RSA::PrintInfo(std::vector<int8192_t>& ecrept_str)
{

	for (const auto e : ecrept_str)
	{

		std::cout << e << " ";

	}

	std::cout << std::endl;

}
*/
