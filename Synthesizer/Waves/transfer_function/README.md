[![UnitTests](https://github.com/borodziejciesla/transfer_function/actions/workflows/UnitTests.yml/badge.svg)](https://github.com/borodziejciesla/transfer_function/actions/workflows/UnitTests.yml)
[![Build Status](https://app.travis-ci.com/borodziejciesla/transfer_function.svg?branch=main)](https://app.travis-ci.com/borodziejciesla/transfer_function)
[![codecov](https://codecov.io/gh/borodziejciesla/transfer_function/branch/main/graph/badge.svg?token=A1E2F7U1A4)](https://codecov.io/gh/borodziejciesla/transfer_function)
[![CodeFactor](https://www.codefactor.io/repository/github/borodziejciesla/transfer_function/badge/main)](https://www.codefactor.io/repository/github/borodziejciesla/transfer_function/overview/main)

![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![GitHub Actions](https://img.shields.io/badge/githubactions-%232671E5.svg?style=for-the-badge&logo=githubactions&logoColor=white)
![TravisCI](https://img.shields.io/badge/travisci-%232B2F33.svg?style=for-the-badge&logo=travis&logoColor=white)

# Transfer Function Library
***
## Overview
---
This is C++ library which provides set of opertions on transfer functions.
Those are operations like:
  * calculate transfer functions of series/parrallel/feedback connection,
  * discretize transfer function,
  * analyse stability,
  * calculate step/impulse response,
  * calculate Bode/Nyquist plot,
  * simulation of discrete transfer function. 

---
### Development enironment
* Programming language:
  * C++20 standard - g++10.3 or higher
* Build system:
  * CMake 3.12 or higher
* Static code analysis:
  * cppcheck 1.90 or higher
  * pygments
* Test coverage:
  * gcov
  * lcov
* Coding style check
  * cpplint 1.5.5 or higher

***
## Usage
---
### Create Transfer Function
```cpp
// Prepare num and den of TF, where tf_core::TransferFunction::CoefficientsVector = std::vector<float>
tf_core::TransferFunction::CoefficientsVector num = {1.0f};
tf_core::TransferFunction::CoefficientsVector den = {1.0f, 2.0f};

auto tf = tf_core::TransferFunction(num, den);
```
---
### Connect Transfer Functions
* Parallel
 
![parallel](https://user-images.githubusercontent.com/44383270/132103271-ec31b6c7-91a4-4671-a71b-27064fa24d38.PNG)

```cpp
// Prepare TF
auto G = tf_core::TransferFunction(num_g, den_g);
auto H = tf_core::TransferFunction(num_h, den_h);

auto parallel_connection = G + H;
//Or
auto parallel_connection = G.Parallel(H);
```
* Serial

![serial](https://user-images.githubusercontent.com/44383270/132103279-a8f55723-878e-42fc-bc0e-01e899a65bb1.PNG)

```cpp
// Prepare TF
auto G = tf_core::TransferFunction(num_g, den_g);
auto H = tf_core::TransferFunction(num_h, den_h);

auto serial_connection = G * H;
//Or
auto serial_connection = G.Serial(H);
```
* Feedback

![feedback](https://user-images.githubusercontent.com/44383270/132103285-fabc907e-1c9c-470b-bf58-558857f3275e.PNG)

```cpp
// Prepare TF
auto G = tf_core::TransferFunction(num_g, den_g);
auto H = tf_core::TransferFunction(num_h, den_h);

auto feedback_connection = G.Feedback(H);
// Or positive feedback
auto feedback_connection = G.Feedback(H, true);
```
---
### Discretize
```cpp
//Define TF
auto G = tf_core::TransferFunction(num_g, den_g);

// Define discretization time and method
float sampling_time = 0.01f;
auto discretization_method = tf_core::DiscretizationMethod::Tustin;

// Discretize
auto discrete_G = G.Discretize(sampling_time, discretization_method);
```
---
### Simulate
```cpp
//Define TF
auto G = tf_core::TransferFunction(num_g, den_g);

// Define signal, where using Signal = std::vector<float>;
Signal input_signal = {...};

// Simulate
auto output_signal = G.Simulate(input_signal);
```
