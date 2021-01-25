#ifndef _FLAT_MARKET_DATA_H_
#define _FLAT_MARKET_DATA_H_

class FlatMarketData {
	public:
		FlatMarketData(double rf_rate, double vola) : m_rf_rate{rf_rate}, m_vola{vola} {}

		double m_rf_rate;
		double m_vola;
};

#endif
