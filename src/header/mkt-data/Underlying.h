#ifndef _UNDERLYING_H_
#define _UNDERLYING_H_

#include <string>

class Underlying {
	public:
		Underlying(const std::string& alias, double refPrice, double spotPrice) 
			: m_alias{alias}, m_ref_price{refPrice}, m_spot_price{spotPrice} {}

	// TODO: I think the string is still not completely immutable
	const std::string& GetAlias() const { return m_alias; }

	double GetReferencePrice() const { return m_ref_price; }
	void SetRefPrice(double refPrice) { m_ref_price = refPrice; }

	double GetSpotPrice() const { return m_spot_price; }
	void SetSpotPrice(double spotPrice) { m_spot_price = spotPrice; }

	private:
		std::string m_alias;
		double m_ref_price = 0;
		double m_spot_price = 0;
};

#endif