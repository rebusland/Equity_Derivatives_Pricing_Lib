#ifndef _UNDERLYING_H_
#define _UNDERLYING_H_

#include <string>

class Underlying {
	public:
		Underlying(const std::string& alias, double refPrice, double spotPrice) 
			: m_alias{alias}, m_ref_price{refPrice}, m_spot_price{spotPrice} {}

	// private: ?
		std::string m_alias;
		double m_ref_price = 0;
		double m_spot_price = 0;
};

#endif