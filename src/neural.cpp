#include "glm/glm.hpp"

#include "neural.h"
#include "util.h"
#include "clock.h"

nn::NeuralNet* net = nullptr;

nn::NeuralNet::NeuralNet(int n)
{
	width = n;
	ScrambleWeights();
}

nn::NeuralNet::~NeuralNet()
{
}

void nn::NeuralNet::SetGamma(Gamma g) {
	gamma = g;
}
Gamma nn::NeuralNet::GetGamma() {
	return gamma;
}
void nn::NeuralNet::SetTarget(Gamma g) {
	target = g;
}
Gamma nn::NeuralNet::GetTarget()
{
	return target;
}

prec nn::NeuralNet::GetWeight(int w)
{
	if (w == 0)
		return bigBias;
	else return weights[w];
}

int nn::NeuralNet::GetWidth()
{
	return width;
}

void nn::NeuralNet::ScrambleWeights()
{
	weights.clear();
	weights.reserve(width);
	biases.clear();
	biases.reserve(width);
	for (int i = 0; i < width; i++)
	{
		weights.push_back(randRange(-1, 1));
		prec bias = (prec)i / (prec)width;
		biases.push_back(bias);
	}
	bigBias = randRange(-1, 1);
}

prec nn::NeuralNet::Compute(prec input)
{
	prec output = bigBias;
	for (int i = 0; i < width; i++) // j = 1 -> n
	{
		output += weights[i] * gamma(input - biases[i]);
	}
	return output;
}

void nn::NeuralNet::PrecomputeAgainstTarget()
{
	computedMinusTargetOutputs.clear();
	computedMinusTargetOutputs.reserve(trainingPrecision);

	for (int i = 0; i < trainingPrecision; i++) //i = 1 -> K
	{
		prec t = (prec)i / (prec)trainingPrecision;
		prec Oi = Compute(t);
		prec Yi = target(t);
		computedMinusTargetOutputs.push_back(Oi - Yi);
	}
}

prec nn::NeuralNet::Error()
{
	prec E = 0;
	for (int i = 0; i < trainingPrecision; i++) //i = 1 -> K
	{
		prec Oi_Yi = computedMinusTargetOutputs[i];
		E += Oi_Yi * Oi_Yi;
	}
	computedError = (float)E;
	return E;
}

prec nn::NeuralNet::SlopeOfBigBias()
{
	prec dEdW0 = 0;
	for (int i = 0; i < trainingPrecision; i++) //i = 1 -> K
	{
		prec Oi_Yi = computedMinusTargetOutputs[i];
		dEdW0 += Oi_Yi;
	}
	return (prec)2 * dEdW0;
}

prec nn::NeuralNet::SlopeOfWeight(int j)
{
	prec dEdWi = 0;
	for (int i = 0; i < trainingPrecision; i++) //i = 1 -> K
	{
		prec Oi_Yi = computedMinusTargetOutputs[i];
		prec t = (prec)i / (prec)trainingPrecision; //input X^i
		prec g = gamma(t - biases[j]);
		dEdWi += Oi_Yi * g;
	}
	return (prec)2 * dEdWi;
}

void nn::NeuralNet::Update()
{
	PrecomputeAgainstTarget();

	for (int j = 0; j < width; j++)
	{
		weights[j] -= learningRate * SlopeOfWeight(j);
	}
	bigBias -= learningRate * SlopeOfBigBias();

	prec E = Error();
}

// ^ Neural Network
/////////////////////////////////////
// v Gammas and Targets

prec nn::Alpha = 6.0f;
prec nn::Sigmoid(prec t)
{
	prec divisor = 1.0f + glm::pow(e_const, -t * nn::Alpha);
	return 1.0f / divisor;
}

prec nn::ReLU(prec t)
{
	return (t >= 0) ? t : (prec)0;
}

prec nn::Direct(prec t)
{
	return t;
}

prec nn::SinGamma(prec t)
{
	return (prec)(sin(pi_const * t) / 2.0 + 0.5);
}

prec nn::DenseSinGamma(prec t)
{
	return (sin(3.0f * pi_const * t - pi_const / 2.0f) + 1.0f) / 2.0f;
}

prec nn::Steps2(prec t)
{
	if(t <= (prec)0.5) return 0;
	else return 1;
}

prec nn::Steps4(prec t)
{
	if (t <= (prec)0.25) return 0;
	if (t <= (prec)0.5) return (prec)0.333333;
	if (t <= (prec)0.75) return (prec)0.666666;
	else return 1;
}

prec nn::Oot(prec t)
{
	return (prec)1.0f / ((prec)20.0f * (t + (prec)0.01));
}

prec nn::T2(prec t)
{
	return t * t;
}

prec nn::Snake(prec t)
{
	prec ret = (glm::sin(1.3f * pi_const * (float)t + (float)clock.time() * 1.0f) + 1.0f) / (2.0f);
	return (3.0f * ret / 5.0f) + 0.2f + 0.2f * glm::sin(clock.time() / 3.0f);
}
