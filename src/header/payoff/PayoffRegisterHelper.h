#ifndef _PAYOFF_REGISTER_HELPER_H_
#define _PAYOFF_REGISTER_HELPER_H_

#include <string>

#include "PayoffFactory.h"

/**
 * A utility class to register a new payoff for the payoff factory.
 * It's hidden to the outside world in a separate namespace.
 */
namespace payoff_registration {

	template<class T_Payoff, class T_Model>
	class PayoffRegisterHelper {
		public:
			PayoffRegisterHelper(std::string payoffId) {
				PayoffFactory& payoffFactory = PayoffFactory::GetInstance();
				payoffFactory.Register(payoffId, PayoffFactory::BuildPayoff<T_Payoff, T_Model>);
			}
	};
}

#endif
