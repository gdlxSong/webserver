/*


�������ҵ�Ŀ�ģ����ַ�������ȡ��ֵ


�������⣺
	�ַ��������ǣ�	"123"
					"abc"
					"0456"
					"0x456a"
					"hsh0x123"
					"0.152"
					"0x.456"

		����һ����Щ�ַ�����
			1. ���е��������ǲ�ͬ��ʽ�ģ�
				��16������0xǰ׺
				��8������0ǰ׺

			2. ���е����������Ԫ�ز�ͬ
				Base16:	0123456789abcdef
				Base2:	01
				Base10: 0123456789

			3. ���е���������С��.
				С������С����.

		���Կ�����ƣ�
			str ->	pre-fix	-> findStart<> -> ����ǰ׺ -\
				->  non-fix -> findStart<>				-\
															-> is_floating_point -> yes ->  ���С���� - > �����Ƿ�����
																				 -> no  ->	return true
				-������stoxxx�ṩһ����ʼ��ַ




*/








#ifndef GDL_STRTO_NUMERIC_HPP
#define GDL_STRTO_NUMERIC_HPP

#include<exception>
#include<string>
#include<optional>
#include<ctype.h>
#include<functional>
#include<type_traits>
#include<memory>
#include<iostream>

namespace gdl {


#ifndef __in__
#define __in__
#endif
#ifndef __out__
#define __out__
#endif
#ifndef __inout__
#define __inout__
#endif


	std::string strtolower(const std::string& str) noexcept {
		std::string s = str;
		for (auto& it : s) {
			if ('A' <= it && it <= 'Z')
				it += 32;
		}
		return s;
	}

	namespace StrToNumeric {





		using position_t = size_t;
		class FloatingPoint {};
		enum Signal { Negative, Positive };
		enum CodingFormat { CF_NonPrefix, CF_MultiPrefix };
		enum NumericType { Base2, Base8, Base10, Base16 };





		Signal checkSignal(const std::string& str, position_t pos) {

			if (0 == pos)
				return Positive;
			else {
				if ('-' == str[pos - 1])
					return Negative;
				return Positive;
			}
		}


		template<class T>
		typename std::enable_if<std::is_floating_point<T>::value, std::optional<position_t> >::type checkPos(const std::string& str, position_t* ppos, std::string pattern, int prefixLength) {


			//CF_MultiPrefix
			//if(prefixLength.size() == 0)
			if (prefixLength > 0) {



				std::string patterndot = pattern + '.';
				position_t pos = *ppos + prefixLength;
				if (pos >= str.size()) {
					//set ppos.
					*ppos = pos;
					return std::nullopt;
				}
				else {
					if (std::string::npos == patterndot.find(str[pos])) {
						//set ppos.
						*ppos = pos;
						return std::nullopt;
					}
					else {
						if ('.' != str[pos])
							return std::optional<position_t>(*ppos);
						else {
							pos += 1;
							if (pos >= str.size()) {
								//set ppos.
								*ppos = pos;
								return std::nullopt;
							}
							else {
								if (std::string::npos == pattern.find(str[pos])) {
									//set ppos.
									*ppos = pos;
									return std::nullopt;
								}
								return std::optional<position_t>(*ppos);
							}
						}
					}
				}
			}
			else {
				//CF_NonPrefix.
				//���ǰһλ�Ƿ�ΪС�����ok��.
				if (0 == *ppos)
					return std::optional<position_t>(*ppos);
				else {
					if ('.' == str[*ppos - 1])
						return std::optional<position_t>(*ppos - 1);
					return std::optional<position_t>(*ppos);
				}
			}
		}

		template<class T>
		typename std::enable_if<!std::is_floating_point<T>::value, std::optional<position_t> >::type checkPos(const std::string& str, position_t* ppos, std::string pattern, int prefixLength) {

			if (0 == prefixLength)
				return std::optional<position_t>(*ppos);

			//CF_MultiPrefix
			if (*ppos + prefixLength >= str.size()) {
				*ppos = *ppos + prefixLength;
				return std::nullopt;
			}
			if (std::string::npos == pattern.find(str[*ppos + prefixLength])) {
				*ppos = *ppos + prefixLength;
				return std::nullopt;
			}
			return std::optional<position_t>(*ppos);
		}


		//findNumeric�����֣����Ƹ�ʽΪ���ַ�(2,10���Ƶ�)�� ���Ƹ�ʽΪ���ַ�(��ʮ������)
		template<CodingFormat CFIndex, NumericType Index>
		typename std::enable_if<CFIndex == CF_NonPrefix, std::optional<position_t> >::type findStart(const std::string& str, const std::string& match, size_t* ppos) {

			//������ǰ׺�ĸ�ʽ������Ѱ����ǰ׺��û�и�ʽֱ��Ѱ�������ݣ��и�ʽ����Ҫcheck.
			auto index = str.find_first_of(match, *ppos);

			if (std::string::npos == index) {
				//not find. and set ppos.
				*ppos = str.size();
				return std::nullopt;
			}
			else
				return std::optional<position_t>(index);

		}

		template<CodingFormat CFIndex, NumericType Index>
		typename std::enable_if<CFIndex == CF_MultiPrefix, std::optional<position_t> >::type findStart(const std::string& str, const std::string& match, size_t* ppos) {

			//������һ������CF_Multi�ļ��裬���Ƕ����ʶλ��������������޹أ���0x123, �������0x�޹�.




			//������ǰ׺�ĸ�ʽ������Ѱ����ǰ׺��û�и�ʽֱ��Ѱ�������ݣ��и�ʽ����Ҫcheck.
			//��Ҫ���ַ���ͳһ��Сд
			std::string s = gdl::strtolower(str);
			auto index = s.find(match, *ppos);

			if (std::string::npos == index) {
				//not find. and set ppos.
				*ppos = s.size();
				return std::nullopt;
			}
			else {

				//�жϸ�ʽ�����Ƿ�������.
				if (index + match.size() >= s.size())
					return std::nullopt;

				return std::optional<position_t>(index);

			}

		}




		//2����
		template<NumericType Index>
		typename std::enable_if<Index == Base2, std::optional<position_t> >::type findNumeric(const std::string& str, size_t* ppos) {

			//���ַ�����Ѱ��ʮ���ƣ��˽��ƣ�������...�����ֵ���ʼ����λ��
			//ppos�ᴩ�������������ǣ�ֻ�����������޸�.
			std::string match = "01";
			return findStart<CF_NonPrefix, Index>(str, match, ppos);
		}

		//8����
		template<NumericType Index>
		typename std::enable_if<Index == Base8, std::optional<position_t> >::type findNumeric(const std::string& str, size_t* ppos) {

			//���ַ�����Ѱ��ʮ���ƣ��˽��ƣ�������...�����ֵ���ʼ����λ��
			//ppos�ᴩ�������������ǣ�ֻ�����������޸�.
			std::string match = "01234567";
			return findStart<CF_NonPrefix, Index>(str, match, ppos);
		}

		//10����
		template<NumericType Index>
		typename std::enable_if<Index == Base10, std::optional<position_t> >::type findNumeric(const std::string& str, size_t* ppos) {

			//���ַ�����Ѱ��ʮ���ƣ��˽��ƣ�������...�����ֵ���ʼ����λ��
			//ppos�ᴩ�������������ǣ�ֻ�����������޸�.
			std::string match = "0123456789";
			return findStart<CF_NonPrefix, Index>(str, match, ppos);
		}


		//ʮ������
		template<NumericType Index>
		typename std::enable_if<Index == Base16, std::optional<position_t> >::type findNumeric(const std::string& str, size_t* ppos) {

			return findStart<CF_MultiPrefix, Index>(str, std::string("0x"), ppos);
		}




		constexpr int getBase(NumericType Index) {

			switch (Index)
			{
			case Base2:
				return 2;
			case Base8:
				return 8;
			case Base10:
				return 10;
			case Base16:
				return 16;
			default:
				return 0;
			}
		}


		std::string getPattern(NumericType Base) {

			//���hash������Ȼ�ǱȽϵ�Ч�ģ��������Ż�
			switch (Base)
			{
			case gdl::StrToNumeric::Base2:
				return "01";
			case gdl::StrToNumeric::Base8:
				return "01234567";
			case gdl::StrToNumeric::Base10:
				return "0123456789";
			case gdl::StrToNumeric::Base16:
				return "0123456789abcdef";
			default:
				return "";
			}
		}


		int getprefixLen(NumericType Base) {

			//���hash������Ȼ�ǱȽϵ�Ч�ģ��������Ż�
			switch (Base)
			{
			case gdl::StrToNumeric::Base2:
				return 0;
			case gdl::StrToNumeric::Base8:
				return 0;
			case gdl::StrToNumeric::Base10:
				return 0;
			case gdl::StrToNumeric::Base16:
				return 2;
			default:
				return 0;
			}
		}

		//Floating.
		template <class destT, class String = std::string, class Func>
		typename std::enable_if<std::is_floating_point<destT>::value, destT>::type stoxxx(Func&& f, const String& __in__ str, size_t* __inout__ ppos = 0, int base = 10) {

			return f(str, ppos);
		}
		//Integer.
		template <class destT, class String = std::string, class Func>
		typename std::enable_if< !std::is_floating_point<destT>::value, destT>::type stoxxx(Func&& f, const String& __in__ str, size_t* __inout__ ppos = 0, int base = 10) {

			return f(str, ppos, base);
		}

		//gdl::stoxxx
		template<NumericType Index, class destT, class String = std::string, class Func>
		std::optional<destT> strTonumeric(Func&& f, const String& __in__ str, size_t* __inout__ ppos = 0) noexcept {


			using valueType = destT;

			//��Ϊppos���ں���ĵ�����ʹ��. ���Բ���Ϊnull, ���Զ�̬����һ��������shared_ptr������
			std::shared_ptr<size_t> shaptr(new size_t{ 0 });
			if (nullptr == ppos) {
				ppos = shaptr.get();
			}


			//str2xxx.
			try {
				//find numeric from &str[0]+*ppos
				std::optional<position_t> pos;
				//check pos.
				do {
					pos = findNumeric<Index>(str, ppos);
					if (pos) {
						//�������ﻹ��������飬Ը������CF_MultiPrefix���ص���0x��λ��...
						*ppos = pos.value();
						pos = checkPos<destT>(str, ppos, getPattern(Index), getprefixLen(Index));
						if (pos)break;
					}
				} while (*ppos < str.size());
				//check pos or (*ppos >= str.size())
				if (!pos)
					return std::nullopt;

				//ppos ��һ��__in__, __out__ ����
				//�жϷ���.
				//set ppos.
				int s = Positive == checkSignal(str, pos.value()) ? 0 : 1;
				valueType value = stoxxx<destT, String, Func>(std::forward<Func>(f), str.c_str() + pos.value() - s, ppos, getBase(Index));
				return std::optional<valueType>(value);
			}
			catch (std::exception& e) {

				std::cout << "what: " << e.what() << std::endl;
				return std::nullopt;
			}

		}

		//std::stoi
		template<class destT, NumericType Index, class String = std::string>
		typename std::enable_if<std::is_same<int, destT>::value, std::optional<destT> >::type strTonumeric(const String& __in__ str, size_t* __inout__ ppos = 0) {

			return strTonumeric<Index, destT, String, int(*)(const std::string&, size_t*, int)>(&std::stoi, str, ppos);
		}

		//std::stol
		template<class destT, NumericType Index, class String = std::string>
		typename std::enable_if<std::is_same<long, destT>::value, std::optional<destT> >::type strTonumeric(const String& __in__ str, size_t* __inout__ ppos = 0) {

			return strTonumeric<Index, destT, String, long(*)(const std::string&, size_t*, int)>(&std::stol, str, ppos);
		}



		//std::stoul
		template<class destT, NumericType Index, class String = std::string>
		typename std::enable_if<std::is_same<unsigned int, destT>::value, std::optional<destT> >::type strTonumeric(const String& __in__ str, size_t* __inout__ ppos = 0) {

			return strTonumeric<Index, destT, String, unsigned int(*)(const std::string&, size_t*, int)>(&std::stoul, str, ppos);
		}


		//std::stoll
		template<class destT, NumericType Index, class String = std::string>
		typename std::enable_if<std::is_same<long long, destT>::value, std::optional<destT> >::type strTonumeric(const String& __in__ str, size_t* __inout__ ppos = 0) {

			return strTonumeric<Index, destT, String, long long(*)(const std::string&, size_t*, int)>(&std::stoll, str, ppos);
		}


		//std::stoull
		template<class destT, NumericType Index, class String = std::string>
		typename std::enable_if<std::is_same<unsigned long long, destT>::value, std::optional<destT> >::type strTonumeric(const String& __in__ str, size_t* __inout__ ppos = 0) {

			return strTonumeric<Index, destT, String, unsigned long long(*)(const std::string&, size_t*, int)>(&std::stoull, str, ppos);
		}




		//std::stof
		template<class destT, NumericType Index, class String = std::string, class _Place>
		typename std::enable_if<std::is_same<float, destT>::value, std::optional<destT> >::type strTonumeric(const String& __in__ str, size_t* __inout__ ppos = 0) {

			return strTonumeric<Index, destT, String, float(*)(const std::string&, size_t*)>(&std::stof, str, ppos);
		}


		//std::stod
		template<class destT, NumericType Index, class String = std::string, class _Place>
		typename std::enable_if<std::is_same<double, destT>::value, std::optional<destT> >::type strTonumeric(const String& __in__ str, size_t* __inout__ ppos = 0) {

			return strTonumeric<Index, destT, String, double(*)(const std::string&, size_t*)>(&std::stod, str, ppos);
		}


		//std::stold
		template<class destT, NumericType Index, class String = std::string, class _Place>
		typename std::enable_if<std::is_same<long double, destT>::value, std::optional<destT> >::type strTonumeric(const String& __in__ str, size_t* __inout__ ppos = 0) {

			return strTonumeric<Index, destT, String, long double(*)(const std::string&, size_t*)>(&std::stold, str, ppos);
		}

		//is_floating_point Base16
		template<class destT, NumericType Index, class String = std::string>
		typename std::enable_if<std::is_floating_point<destT>::value, typename std::enable_if<Base16 == Index, std::optional<destT> >::type>::type strTonumeric(const String& __in__ str, size_t* __inout__ ppos = 0) {

			return strTonumeric<destT, Base16, String, FloatingPoint>(str, ppos);
		}

		//is_floating_point Base...other: Base2, Base8, Base10		:ԭ������floatֻ֧��10��16����
		template<class destT, NumericType Index, class String = std::string>
		typename std::enable_if<std::is_floating_point<destT>::value, typename std::enable_if<Base16 != Index, std::optional<destT> >::type>::type strTonumeric(const String& __in__ str, size_t* __inout__ ppos = 0) {

			return strTonumeric<destT, Base10, String, FloatingPoint>(str, ppos);
		}




	}





}





#endif