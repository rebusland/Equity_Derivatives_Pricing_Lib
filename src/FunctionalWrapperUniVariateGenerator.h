#ifndef _FUNCTIONAL_WRAPPER_UNIVARIATE_GENERATOR_H_
#define _FUNCTIONAL_WRAPPER_UNIVARIATE_GENERATOR_H_

#include <algorithm>
#include <functional>
#include <vector>

#include "UniVariateNumbersGenerator.h"

using generator_func = std::function<double ()>;

/**
 * Decorator wrapping a functional generator into UniVariateNumbersGenerator's interface:
 * this allows to wrap std library random numbers generators into UniVariateNumbersGenerator's interface.
 * TODO IMPORTANT:
 *  - The default seed (set in the base class) is actually NOT used: 
 * 	  the seed is implicitly "embedded" inside generator_func; thus, is not possible to reset it.
 */
class FunctionalWrapperUniVariateGenerator : public UniVariateNumbersGenerator {
	public:
		FunctionalWrapperUniVariateGenerator(unsigned int seqSize, generator_func generator) :
			UniVariateNumbersGenerator(seqSize),
			m_generator{generator} {}

		double GenerateOne() override {
			return m_generator();
		}

		std::vector<double> GenerateSequence() override {
			std::vector<double> numberSequence(m_sequence_size);
			// wrapping the generator into a ref object is necessary, otherwise a copy is made
			std::generate(numberSequence.begin(), numberSequence.end(), std::ref(m_generator));
			return numberSequence;
		}

	private:
		generator_func m_generator;
};

#endif
