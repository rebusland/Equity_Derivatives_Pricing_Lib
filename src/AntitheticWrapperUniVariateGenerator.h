#ifndef _ANTITHETIC_WRAPPER_UNIVARIATE_GENERATOR_H_
#define _ANTITHETIC_WRAPPER_UNIVARIATE_GENERATOR_H_

#include <algorithm>
#include <functional>
#include <vector>

#include "UniVariateNumbersGenerator.h"
#include "ProjectException.h"

/**
 * A decorator to an arbitrary number generator functor, producing antithetic numbers.
 */
class AntitheticWrapperUniVariateGenerator : public UniVariateNumbersGenerator {
	public:
		// TODO implement the factory pattern to reduce this mess of constructors
		AntitheticWrapperUniVariateGenerator(
			unsigned int seqSize,
			double seed,
			UniVariateNumbersGenerator* innerGenerator,
			std::function<double (double)> antitheticRule = std::negate<double>()
		) :	UniVariateNumbersGenerator(seqSize, seed),
			m_inner_generator{innerGenerator},
			m_antithetic_mapping_rule{antitheticRule} {
				m_inner_generator->SetSeed(seed);

				m_antithetic_sequence.resize(m_sequence_size);
			}

		AntitheticWrapperUniVariateGenerator(
			unsigned int seqSize,
			UniVariateNumbersGenerator* innerGenerator,
			std::function<double (double)> antitheticRule = std::negate<double>()
		):
			UniVariateNumbersGenerator(seqSize),
			m_inner_generator{innerGenerator},
			m_antithetic_mapping_rule{antitheticRule} {
				m_antithetic_sequence.resize(m_sequence_size);
			}

		double GenerateOne() override {
			THROW_PROJECT_EXCEPTION("Unsupported method for AntitheticGeneratorWrapper");
		}

		std::vector<double> GenerateSequence() override {
			if (m_toggle) {
				m_toggle = not m_toggle;
				return m_antithetic_sequence;
			}

			m_toggle = not m_toggle;
			std::vector<double> sequenceVec = m_inner_generator->GenerateSequence();
			// apply antithetic transformation
			std::transform(sequenceVec.begin(), sequenceVec.end(),
				m_antithetic_sequence.begin(), m_antithetic_mapping_rule);
			return sequenceVec;
		}

	private:
		// TODO use smart pointer instead?
		UniVariateNumbersGenerator* m_inner_generator;
		std::vector<double> m_antithetic_sequence;

		// the rule to determine the antithetic element (by default is the opposite number)
		std::function<double (double)> m_antithetic_mapping_rule;

		bool m_toggle = false;
};

#endif
