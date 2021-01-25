#include "variate-generator/UniVariateNumbersGeneratorFactory.h"

#include "variate-generator/GaussianVariatesGenerator.h"
#include "variate-generator/AntitheticWrapperUniVariateGenerator.h"

#include "util/ProjectException.h"

std::unique_ptr<UniVariateNumbersGenerator> UniVariateNumbersGeneratorFactory::CreateVariateGenerator(
	VariateGeneratorEnum variateGeneratorType,
	bool shouldApplyAntitheticWrapper,
	unsigned int seqSize,
	double seed
) {
	std::unique_ptr<UniVariateNumbersGenerator> generator = nullptr;
	switch (variateGeneratorType) {
		case VariateGeneratorEnum::STD_GAUSS:
			generator = std::make_unique<GaussianVariatesGenerator>(seqSize, seed);
			break;
		default:
			THROW_PROJECT_EXCEPTION("Invalid generatorId");
	}

	if (shouldApplyAntitheticWrapper) {
		return std::make_unique<AntitheticWrapperUniVariateGenerator>(seqSize, seed, std::move(generator));
	}

	return generator;
}
