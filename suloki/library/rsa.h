#ifndef _SULOKI_RSA_H_
#define _SULOKI_RSA_H_
/*
#include<iostream>
#include <string>
#include <vector>
#include<time.h>
#include<fstream>

//boost库的大数
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/random.hpp>
#include <boost/multiprecision/miller_rabin.hpp>//素数

namespace bm = boost::multiprecision;
namespace br = boost::random;

typedef bm::number< bm::cpp_int_backend<8192, 8192, bm::signed_magnitude, bm::unchecked, void> > int8192_t;

struct Key
{
	//公钥(ekey, pkey): (e,n)
	int8192_t nkey;
	int8192_t ekey;
	//私钥(dkey, pkey): (d, n)
	int8192_t dkey;
};

class RSA
{
public:
	RSA();
	Key GetKey();
	void FileEcrept(const char* plain_file_in, const char* ecrept_file_out,
		int8192_t ekey, int8192_t pkey);
	void FileDecrept(const char* ecrept_file_in, const char* plain_file_out,
		int8192_t dkey, int8192_t pkey);
	std::vector<int8192_t> Ecrept(std::string& str_in, int8192_t ekey, int8192_t pkey);
	std::string Decrept(std::vector<int8192_t>& ecrept_str, int8192_t dkey, int8192_t pkey);
	void PrintInfo(std::vector<int8192_t>& ecrept_str);

private:
	//加密与解密单个信息
	int8192_t ecrept(int8192_t msg, int8192_t key, int8192_t pkey);
	//产生大素数
	int8192_t ProducePrime();
	//判断是否是素数
	bool is_prime(int8192_t prime);
	//产生钥匙
	void ProduceKeys();
	int8192_t ProduceNkey(int8192_t prime1, int8192_t prime2);
	int8192_t ProduceOrla(int8192_t prime1, int8192_t prime2);
	int8192_t ProduceEkey(int8192_t orla);
	//产生公约数
	int8192_t ProduceGcd(int8192_t ekey, int8192_t orla);
	int8192_t ProduceGcd(int8192_t ekey, int8192_t orla, int8192_t& x, int8192_t& y);

	int8192_t producedkey(int8192_t ekey, int8192_t orla);
private:
	Key key_;
};
*/
#endif
