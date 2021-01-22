#ifndef _GAUSSIAN_VARIATES_GENERATOR_H_
#define _GAUSSIAN_VARIATES_GENERATOR_H_

#include <algorithm>
#include <functional>
#include <random>

#include "UniVariateNumbersGenerator.h"

/**
 * TODO
 *  - Random numbers engines (generators) in <random> header should be reproducible 
 *    accross compilers; however, distributions seems not!
 *    Should I write my own implementation of normal distribution? 
 *	  Or implement the Box-Muller transformation? Or start exploiting the Boost library?
 */
class GaussianVariatesGenerator : public UniVariateNumbersGenerator {
	public:
		GaussianVariatesGenerator(unsigned int seqSize, double seed = 1)
			: UniVariateNumbersGenerator(seqSize, seed) {
			// NB it's important to pass the distribution and the rng by reference
			m_normal_variates_generator_func = std::bind(std::ref(m_normal_distr), std::ref(m_uniform_rng));
		}

		virtual double GenerateOne() override {
			return m_normal_variates_generator_func();
		}

		virtual std::vector<double> GenerateSequence() override {
			std::vector<double> numberSequence(m_sequence_size);
			// wrapping the generator into a ref object is necessary, otherwise a copy is made
			std::generate(numberSequence.begin(), numberSequence.end(), std::ref(m_normal_variates_generator_func));
			return numberSequence;		
		}

		virtual std::unique_ptr<UniVariateNumbersGenerator> Clone() const override {
			// we use this form (instead of make_unique) in order to exploit the private constructor
			return std::unique_ptr<GaussianVariatesGenerator>(new GaussianVariatesGenerator(*this));
		}

		void SetSeed(double seed) override {
			// this does not actually have any effect, it's done just for consistency
			UniVariateNumbersGenerator::SetSeed(seed);

			m_uniform_rng.seed(seed);
		}

	private:
		// private copy constructor for the sole purpose of cloning: we have to assure that it is not copied the reference
		// to the normal_variate_generator_func otherwise the very same rng would be used by the clone. We have to guarantee
		// that a new functional wrapper to the generator (but with the same characteristics) is assigned to the clone
		GaussianVariatesGenerator(const GaussianVariatesGenerator& gen) : UniVariateNumbersGenerator(gen.m_sequence_size, gen.m_seed) {
			// the default copy constructor would have copied the functional wrapper's reference, instead of creating a brand new one.
			m_normal_variates_generator_func = std::bind(std::ref(m_normal_distr), std::ref(m_uniform_rng));
		}

		// Park-Miller linear_congruential_engine
		std::minstd_rand m_uniform_rng;

		// by default on creation is N(0, 1) as needed
		std::normal_distribution<double> m_normal_distr;

		std::function<double ()> m_normal_variates_generator_func;		
};

#endif
