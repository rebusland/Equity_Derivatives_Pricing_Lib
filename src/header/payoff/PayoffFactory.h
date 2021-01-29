#ifndef _PAYOFF_FACTORY_H_
#define _PAYOFF_FACTORY_H_

#include <functional>
#include <map>
#include <memory>
#include <vector>

#include "util/pattern/Singleton.h"

#include "derivative/Derivative.h"
#include "Payoff.h"
#include "util/ProjectException.h"

// forward declaration of PayoffRegisterHelper
namespace payoff_registration {
	template<class T_friend, class U_friend>
	class PayoffRegisterHelper;
}

#ifndef _DATE_DEF_
#define _DATE_DEF_
using _Date = long;
#endif

using PayoffBuilderFunc = std::function<std::unique_ptr<Payoff> (Derivative*, const std::vector<double>&, std::string)>;

class PayoffFactory : public Singleton<PayoffFactory> {
	public:
		/*
		 * Get an instance of the payoff specified by payoffId
		 */
		std::unique_ptr<Payoff> CreatePayoff(
			std::string payoffId,
			Derivative* derivativeParams,
			const std::vector<double>& discounts
		);

	private:
		/*
		 * the constructor for MyFactory is private so we need the friend declaration to allow
		 * the Singleton class to create the one object from the factory.
		 */
		friend class Singleton<PayoffFactory>;

		template<class T_friend, class U_friend>
		friend class payoff_registration::PayoffRegisterHelper;

		PayoffFactory() {};

		template<class T_Payoff, class T_Model>
		static std::unique_ptr<Payoff> BuildPayoff(Derivative* params, const std::vector<double>& discounts, std::string payoffId) {
			// check the datamodel of the derivative parameters is of the right type and build object
			T_Model* paramModelPtr = dynamic_cast<T_Model*>(params);
			if (paramModelPtr == nullptr) {
				THROW_PROJECT_EXCEPTION("Derivative model inconsistent with the payoff type: " + payoffId);
			}
			return std::unique_ptr<T_Payoff>(new T_Payoff(*paramModelPtr, discounts));
		}

		void Register(std::string payoffId, PayoffBuilderFunc builderFunc);

		std::map<std::string, PayoffBuilderFunc> m_payoffId_to_builder_map;
};

#endif
