#include "TransferFunction.h"

int main()
{
	TF::Symbolic_eqn<1> numerator{ "K" };

	TF::Symbolic_eqn<4> denominator{ "", "( b * R ) + K ^ 2 ", "( J * R ) + (L * b)", "J * L" };

	TF::Variables<5> variables{ {'J', 0.043}, {'b', 0.000011879}, {'K', 0.42369}, {'R', 11}, {'L', 0.00000275} };

	TF::TransferFunction tf = transferFunction(numerator, denominator, variables);
	tf.print();
	tf.bode(-2, 8);

	class Pause
	{
	public:
		~Pause() { system("pause"); }
	};

	Pause pause;

	return 0;
}