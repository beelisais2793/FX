# Transfer-Function  
  
### Define symbolic transfer function and variables to return numeric equation.  
See *TFClient.cpp* for an example using a DC motor and flywheel.  

### Use *Symbolic_eqn<>* to generate a string symbolic equation where terms are separated by commas in the initializer list.  
Ex. TF::Symbolic_eqn<4> denominator{ "", "( b * R ) + K ^ 2 ", "( J * R ) + (L * b)", "J * L" };  
Note: Spaces don't matter.  
  
### Define variables using *Variables<>*.  
Ex. TF::Variables<5> variables{ {'J', 0.043}, {'b', 0.000011879}, {'K', 0.42369}, {'R', 11}, {'L', 0.00000275} };  
  
### Create numeric transfer function by using the *transferFunction()* function which returns a TransferFunction object.  
Ex. TF::TransferFunction tf = transferFunction(numerator, denominator, variables);  
Note: TransferFunction objects can be defined using numeric values in the initializer list.  
  
### The *print()* member function in TransferFunction allows for printing to the command prompt.  
  
### The *bode(int upperBound, int lowerBound)* member function takes a lower bound of 10^(*lowerBound*) and upper bound of 10^(*upperBound*)  
The function creates a csv with the frequency response defined by the upper and lower bounds.  
Ex. tf.bode(-2, 8);
