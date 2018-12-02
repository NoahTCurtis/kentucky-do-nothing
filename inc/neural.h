#pragma once

#include <vector>

#define e_const (prec)2.718281828459045f
#define pi_const (prec)3.14159265358f

typedef float prec;
typedef prec(*Gamma)(prec);

namespace nn
{
	class NeuralNet
	{
	public:
		NeuralNet(int n);
		~NeuralNet();
		void ScrambleWeights();
		void SetGamma(Gamma g);
		Gamma GetGamma();
		void SetTarget(Gamma g);
		Gamma GetTarget();
		prec GetWeight(int w);
		int GetWidth();

		prec Compute(prec input);

		void PrecomputeAgainstTarget();
		prec Error();
		prec SlopeOfBigBias();
		prec SlopeOfWeight(int j);

		void Update();

		prec learningRate = 0.0003f;
		int trainingPrecision = 100; //K
		float computedError = 0.0f;

	private:
		int width;
		Gamma gamma; //activation function
		Gamma target; //function to mimic

		std::vector<prec> weights; //Wn
		std::vector<prec> biases; //Bn
		std::vector<prec> computedMinusTargetOutputs; //Ok-Yk
		prec bigBias; //W0
	};

	extern prec Alpha;
	prec Sigmoid(prec t);
	prec ReLU(prec t);
	prec Direct(prec t);
	prec SinGamma(prec t);
	prec DenseSinGamma(prec t);
	prec Steps2(prec t);
	prec Steps4(prec t);
	prec Oot(prec t);
	prec T2(prec t);
	prec Snake(prec t);
};

extern nn::NeuralNet* net;