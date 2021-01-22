#ifndef _UNIVARIATE_NUMBERS_GENERATOR_FACTORY_H_
#define _UNIVARIATE_NUMBERS_GENERATOR_FACTORY_H_

#include "Singleton.h"

#include <memory>

#include "MonteCarloSettings.h"
#include "UniVariateNumbersGenerator.h"

class UniVariateNumbersGeneratorFactory : public Singleton<UniVariateNumbersGeneratorFactory> {
	public:
		/*
		 * Get an instance of the generator specified by the generatorId
		 */
		std::unique_ptr<UniVariateNumbersGenerator> CreateVariateGenerator(
			VariateGeneratorEnum variateGeneratorType,
			bool shouldApplyAntitheticWrapper,
			unsigned int seqSize,
			double seed = 1
		);

	private:
		/*
		 * the constructor for MyFactory is private so we need the friend declaration to allow
		 * the Singleton class to create the one object from the factory.
		 */
		friend class Singleton<UniVariateNumbersGeneratorFactory>;

		UniVariateNumbersGeneratorFactory() {};
};

#endif
