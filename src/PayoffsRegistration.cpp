#include "PayoffRegisterHelper.h"

#include "AsianOption.h"
#include "AsianPayoff.h"

#include "VanillaOption.h"
#include "PlainVanillaPayoff.h"

/**
 * RAII is exploited to register the supported payoffs outside the main function's context.
 * The helper class PayoffRegisterHelper<T_Payoff, T_Model>(payoffId) associates the payoffId to 
 * the corresponding payoff T_Payoff, and in turn the payoff to the proper derivative datamodel T_Model.
 */
namespace payoff_registration {

	PayoffRegisterHelper<AsianPayoff, AsianOption> registerAsian("ASIAN");
	PayoffRegisterHelper<PlainVanillaPayoff, VanillaOption> registerPlainVanilla("EUROPEAN_STD");

}
