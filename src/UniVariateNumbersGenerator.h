#ifndef _UNI_VARIATE_NUMBERS_GENERATOR_H_
#define _UNI_VARIATE_NUMBERS_GENERATOR_H_

#include "Clonable.h"

#include <memory>
#include <vector>

/**
 * Wrapper to an arbitrary number generator, providing a common interface to generate numbers for MonteCarlo simulations.
 * The sequence could be of (pseudo)random numbers or of low-discrepancy (i.e. quasi-random) numbers.
 */
class UniVariateNumbersGenerator : public Clonable<UniVariateNumbersGenerator> {
	public:
		/**
		 * Quoting M. Joshi from "C++ design patterns and derivative pricing":
		 * << Generally, when working with any Monte Carlo simulation, the simulation will
		 * have a dimensionality which is the number of random draws needed to simulate
		 * one path. This number is equal to the number of variables of integration in the
		 * underlying integral which we are trying to approximate. When working with 
		 * low-discrepancy numbers it is essential that the generator know the dimensionality 
		 * as the generator has to be set up specifically for each choice of dimension.>>
		 */
		UniVariateNumbersGenerator(unsigned int seqSize, double seed = 1) :
			m_sequence_size{seqSize}, m_seed{seed} {}

		virtual void SetSeed(double seed) {
			m_seed = seed;
		}

		/**
		 * Returns a single number
		 */
		virtual double GenerateOne() = 0;

		/**
		 * Returns a sequence of "m_sequence_size" numbers
		 */
		virtual std::vector<double> GenerateSequence() = 0;

		// TODO (?)
		// virtual void ResetSequenceSize(unsigned int newSeqSize) const = 0;

		virtual ~UniVariateNumbersGenerator() {};

	protected:
		unsigned int m_sequence_size;
		double m_seed;
};

#endif
