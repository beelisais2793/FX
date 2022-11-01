#pragma once

namespace Std {

      struct RandomBase : public StdObject
      {
            virtual unsigned generate() = 0;       
      };

      struct RandomMinStd : public RandomBase
      {
      // swig is not allowed to see this
      private:
            unsigned seed;
            std::minstd_rand0 generator;
      public:

            void seed_engine() 
            {
                  seed = std::chrono::system_clock::now().time_since_epoch().count();
                  generator.seed(seed);
            }

            unsigned min() { return generator.min(); }
            unsigned max() { return generator.max(); }

            unsigned generate()
            {        
                  return generator();
            }    

            unsigned randint(unsigned min, unsigned max)
            {
                  return (max-min)*rand() + min;
            }
            double rand()
            {        
                  return (double)generate() / (double)max();
            }

            int uniform_int_distribution(int min, int max)
            {
                  std::uniform_int_distribution<int> distribution(min,max);
                  return distribution(generator);
            }
            double uniform_real_distribution(double min, double max)
            {
                  std::uniform_real_distribution<double> distribution(min,max);
                  return distribution(generator);
            }
            int binomial_distribution(double min, double max)
            {
                  std::binomial_distribution<int> distribution(min,max);
                  return distribution(generator);
            }    
            double cauchy_distribution(double min, double max)
            {
                  std::cauchy_distribution<double> distribution(min,max);
                  return distribution(generator);
            }
            double chi_squared_distribution(double x)
            {
                  std::chi_squared_distribution<double> distribution(x);
                  return distribution(generator);
            }
            int discrete_distribution(int min, int max)
            {
                  std::discrete_distribution<int> distribution(min,max);
                  return distribution(generator);
            }
            double exponential_distribution(double x)
            {
                  std::exponential_distribution<double> distribution(x);
                  return distribution(generator);
            }
            double fisher_distribution(double min, double max)
            {
                  std::fisher_f_distribution<double> distribution(min,max);
                  return distribution(generator);
            }
            double gamma_distribution(double min, double max)
            {
                  std::gamma_distribution<double> distribution(min,max);
                  return distribution(generator);
            }
            int geometric_distribution(double x)
            {
                  std::geometric_distribution<int> distribution(x);
                  return distribution(generator);
            }
            double lognormal_distribution(double min, double max)
            {
                  std::lognormal_distribution<double> distribution(min,max);
                  return distribution(generator);
            }
            int negative_binomial_distribution(int min, int max)
            {
                  std::negative_binomial_distribution<int> distribution(min,max);
                  return distribution(generator);
            }
            double normal_distribution(double a, double b)
            {
                  std::normal_distribution<double> distribution(a,b);
                  return distribution(generator);
            }
            double piecewise_constant_distribution()
            {
                  std::piecewise_constant_distribution<double> distribution;
                  return distribution(generator);
            }
            double piecewise_linear_distribution()
            {
                  std::piecewise_linear_distribution<double> distribution;
                  return distribution(generator);
            }
            int poisson_distribution(double p)
            {
                  std::poisson_distribution<int> distribution(p);
                  return distribution(generator);
            }
            
      };


      struct RandomMersenne : public RandomBase
      {
      private:
            unsigned seed;
            std::mt19937 generator;
      public:
            void seed_engine() 
            {
                  seed = std::chrono::system_clock::now().time_since_epoch().count();
                  generator.seed(seed);
            }

            unsigned min() { return generator.min(); }
            unsigned max() { return generator.max(); }

            unsigned generate()
            {        
                  return generator();
            }    

            unsigned randint(unsigned min, unsigned max)
            {
                  return (max-min)*rand() + min;
            }
            double rand()
            {        
                  return (double)generate() / (double)max();
            }

            int uniform_int_distribution(int min, int max)
            {
                  std::uniform_int_distribution<int> distribution(min,max);
                  return distribution(generator);
            }
            double uniform_real_distribution(double min, double max)
            {
                  std::uniform_real_distribution<double> distribution(min,max);
                  return distribution(generator);
            }
            int binomial_distribution(double min, double max)
            {
                  std::binomial_distribution<int> distribution(min,max);
                  return distribution(generator);
            }    
            double cauchy_distribution(double min, double max)
            {
                  std::cauchy_distribution<double> distribution(min,max);
                  return distribution(generator);
            }
            double chi_squared_distribution(double x)
            {
                  std::chi_squared_distribution<double> distribution(x);
                  return distribution(generator);
            }
            int discrete_distribution(int min, int max)
            {
                  std::discrete_distribution<int> distribution(min,max);
                  return distribution(generator);
            }
            double exponential_distribution(double x)
            {
                  std::exponential_distribution<double> distribution(x);
                  return distribution(generator);
            }
            double fisher_distribution(double min, double max)
            {
                  std::fisher_f_distribution<double> distribution(min,max);
                  return distribution(generator);
            }
            double gamma_distribution(double min, double max)
            {
                  std::gamma_distribution<double> distribution(min,max);
                  return distribution(generator);
            }
            int geometric_distribution(double x)
            {
                  std::geometric_distribution<int> distribution(x);
                  return distribution(generator);
            }
            double lognormal_distribution(double min, double max)
            {
                  std::lognormal_distribution<double> distribution(min,max);
                  return distribution(generator);
            }
            int negative_binomial_distribution(int min, int max)
            {
                  std::negative_binomial_distribution<int> distribution(min,max);
                  return distribution(generator);
            }
            double normal_distribution(double a, double b)
            {
                  std::normal_distribution<double> distribution(a,b);
                  return distribution(generator);
            }
            double piecewise_constant_distribution()
            {
                  std::piecewise_constant_distribution<double> distribution;
                  return distribution(generator);
            }
            double piecewise_linear_distribution()
            {
                  std::piecewise_linear_distribution<double> distribution;
                  return distribution(generator);
            }
            int poisson_distribution(double p)
            {
                  std::poisson_distribution<int> distribution(p);
                  return distribution(generator);
            }

      };


      struct RandomMersenne64 : public RandomBase
      {
      private:
            unsigned seed;
            std::mt19937_64 generator;
      public:
            void seed_engine() 
            {
                  seed = std::chrono::system_clock::now().time_since_epoch().count();
                  generator.seed(seed);
            }

            long unsigned min() { return generator.min(); }
            long unsigned max() { return generator.max(); }

            unsigned generate()
            {        
                  return generator();
            }    

            unsigned randint(unsigned min, unsigned max)
            {
                  return (max-min)*rand() + min;
            }
            double rand()
            {        
                  return (double)generate() / (double)max();
            }
                  int uniform_int_distribution(int min, int max)
            {
                  std::uniform_int_distribution<int> distribution(min,max);
                  return distribution(generator);
            }
            double uniform_real_distribution(double min, double max)
            {
                  std::uniform_real_distribution<double> distribution(min,max);
                  return distribution(generator);
            }
            int binomial_distribution(double min, double max)
            {
                  std::binomial_distribution<int> distribution(min,max);
                  return distribution(generator);
            }    
            double cauchy_distribution(double min, double max)
            {
                  std::cauchy_distribution<double> distribution(min,max);
                  return distribution(generator);
            }
            double chi_squared_distribution(double x)
            {
                  std::chi_squared_distribution<double> distribution(x);
                  return distribution(generator);
            }
            int discrete_distribution(int min, int max)
            {
                  std::discrete_distribution<int> distribution(min,max);
                  return distribution(generator);
            }
            double exponential_distribution(double x)
            {
                  std::exponential_distribution<double> distribution(x);
                  return distribution(generator);
            }
            double fisher_distribution(double min, double max)
            {
                  std::fisher_f_distribution<double> distribution(min,max);
                  return distribution(generator);
            }
            double gamma_distribution(double min, double max)
            {
                  std::gamma_distribution<double> distribution(min,max);
                  return distribution(generator);
            }
            int geometric_distribution(double x)
            {
                  std::geometric_distribution<int> distribution(x);
                  return distribution(generator);
            }
            double lognormal_distribution(double min, double max)
            {
                  std::lognormal_distribution<double> distribution(min,max);
                  return distribution(generator);
            }
            int negative_binomial_distribution(int min, int max)
            {
                  std::negative_binomial_distribution<int> distribution(min,max);
                  return distribution(generator);
            }
            double normal_distribution(double a, double b)
            {
                  std::normal_distribution<double> distribution(a,b);
                  return distribution(generator);
            }
            double piecewise_constant_distribution()
            {
                  std::piecewise_constant_distribution<double> distribution;
                  return distribution(generator);
            }
            double piecewise_linear_distribution()
            {
                  std::piecewise_linear_distribution<double> distribution;
                  return distribution(generator);
            }
            int poisson_distribution(double p)
            {
                  std::poisson_distribution<int> distribution(p);
                  return distribution(generator);
            }

      };



      struct RandomRanLux24 : public RandomBase
      {
      private:      
            unsigned seed;
            std::ranlux24 generator;
      public:
            void seed_engine() 
            {
                  seed = std::chrono::system_clock::now().time_since_epoch().count();
                  generator.seed(seed);
            }

            unsigned min() { return generator.min(); }
            unsigned max() { return generator.max(); }

            unsigned generate()
            {        
                  return generator();
            }    

            unsigned randint(unsigned min, unsigned max)
            {
                  return (max-min)*rand() + min;
            }
            double rand()
            {        
                  return (double)generate() / (double)max();
            }
                  int uniform_int_distribution(int min, int max)
            {
                  std::uniform_int_distribution<int> distribution(min,max);
                  return distribution(generator);
            }
            double uniform_real_distribution(double min, double max)
            {
                  std::uniform_real_distribution<double> distribution(min,max);
                  return distribution(generator);
            }
            int binomial_distribution(double min, double max)
            {
                  std::binomial_distribution<int> distribution(min,max);
                  return distribution(generator);
            }    
            double cauchy_distribution(double min, double max)
            {
                  std::cauchy_distribution<double> distribution(min,max);
                  return distribution(generator);
            }
            double chi_squared_distribution(double x)
            {
                  std::chi_squared_distribution<double> distribution(x);
                  return distribution(generator);
            }
            int discrete_distribution(int min, int max)
            {
                  std::discrete_distribution<int> distribution(min,max);
                  return distribution(generator);
            }
            double exponential_distribution(double x)
            {
                  std::exponential_distribution<double> distribution(x);
                  return distribution(generator);
            }
            double fisher_distribution(double min, double max)
            {
                  std::fisher_f_distribution<double> distribution(min,max);
                  return distribution(generator);
            }
            double gamma_distribution(double min, double max)
            {
                  std::gamma_distribution<double> distribution(min,max);
                  return distribution(generator);
            }
            int geometric_distribution(double x)
            {
                  std::geometric_distribution<int> distribution(x);
                  return distribution(generator);
            }
            double lognormal_distribution(double min, double max)
            {
                  std::lognormal_distribution<double> distribution(min,max);
                  return distribution(generator);
            }
            int negative_binomial_distribution(int min, int max)
            {
                  std::negative_binomial_distribution<int> distribution(min,max);
                  return distribution(generator);
            }
            double normal_distribution(double a, double b)
            {
                  std::normal_distribution<double> distribution(a,b);
                  return distribution(generator);
            }
            double piecewise_constant_distribution()
            {
                  std::piecewise_constant_distribution<double> distribution;
                  return distribution(generator);
            }
            double piecewise_linear_distribution()
            {
                  std::piecewise_linear_distribution<double> distribution;
                  return distribution(generator);
            }
            int poisson_distribution(double p)
            {
                  std::poisson_distribution<int> distribution(p);
                  return distribution(generator);
            }

      };


      struct RandomRanLux48 : public RandomBase
      {
      private:
      
            unsigned seed;
            std::ranlux48 generator;
      public:
            void seed_engine() 
            {
                  seed = std::chrono::system_clock::now().time_since_epoch().count();
                  generator.seed(seed);
            }

            long unsigned min() { return generator.min(); }
            long unsigned max() { return generator.max(); }

            unsigned generate()
            {        
                  return generator();
            }    

            unsigned randint(unsigned min, unsigned max)
            {
                  return (max-min)*rand() + min;
            }
            double rand()
            {        
                  return (double)generate() / (double)max();
            }
                  int uniform_int_distribution(int min, int max)
            {
                  std::uniform_int_distribution<int> distribution(min,max);
                  return distribution(generator);
            }
            double uniform_real_distribution(double min, double max)
            {
                  std::uniform_real_distribution<double> distribution(min,max);
                  return distribution(generator);
            }
            int binomial_distribution(double min, double max)
            {
                  std::binomial_distribution<int> distribution(min,max);
                  return distribution(generator);
            }    
            double cauchy_distribution(double min, double max)
            {
                  std::cauchy_distribution<double> distribution(min,max);
                  return distribution(generator);
            }
            double chi_squared_distribution(double x)
            {
                  std::chi_squared_distribution<double> distribution(x);
                  return distribution(generator);
            }
            int discrete_distribution(int min, int max)
            {
                  std::discrete_distribution<int> distribution(min,max);
                  return distribution(generator);
            }
            double exponential_distribution(double x)
            {
                  std::exponential_distribution<double> distribution(x);
                  return distribution(generator);
            }
            double fisher_distribution(double min, double max)
            {
                  std::fisher_f_distribution<double> distribution(min,max);
                  return distribution(generator);
            }
            double gamma_distribution(double min, double max)
            {
                  std::gamma_distribution<double> distribution(min,max);
                  return distribution(generator);
            }
            int geometric_distribution(double x)
            {
                  std::geometric_distribution<int> distribution(x);
                  return distribution(generator);
            }
            double lognormal_distribution(double min, double max)
            {
                  std::lognormal_distribution<double> distribution(min,max);
                  return distribution(generator);
            }
            int negative_binomial_distribution(int min, int max)
            {
                  std::negative_binomial_distribution<int> distribution(min,max);
                  return distribution(generator);
            }
            double normal_distribution(double a, double b)
            {
                  std::normal_distribution<double> distribution(a,b);
                  return distribution(generator);
            }
            double piecewise_constant_distribution()
            {
                  std::piecewise_constant_distribution<double> distribution;
                  return distribution(generator);
            }
            double piecewise_linear_distribution()
            {
                  std::piecewise_linear_distribution<double> distribution;
                  return distribution(generator);
            }
            int poisson_distribution(double p)
            {
                  std::poisson_distribution<int> distribution(p);
                  return distribution(generator);
            }

      };

      struct RandomKnuth : public RandomBase
      {
      private:      
            unsigned seed;
            std::knuth_b generator;
      public:
            void seed_engine() 
            {
                  seed = std::chrono::system_clock::now().time_since_epoch().count();
                  generator.seed(seed);
            }

            long unsigned min() { return generator.min(); }
            long unsigned max() { return generator.max(); }

            unsigned generate()
            {        
                  return generator();
            }    

            unsigned randint(unsigned min, unsigned max)
            {
                  return (max-min)*rand() + min;
            }
            double rand()
            {        
                  return (double)generate() / (double)max();
            }
                  int uniform_int_distribution(int min, int max)
            {
                  std::uniform_int_distribution<int> distribution(min,max);
                  return distribution(generator);
            }
            double uniform_real_distribution(double min, double max)
            {
                  std::uniform_real_distribution<double> distribution(min,max);
                  return distribution(generator);
            }
            int binomial_distribution(double min, double max)
            {
                  std::binomial_distribution<int> distribution(min,max);
                  return distribution(generator);
            }    
            double cauchy_distribution(double min, double max)
            {
                  std::cauchy_distribution<double> distribution(min,max);
                  return distribution(generator);
            }
            double chi_squared_distribution(double x)
            {
                  std::chi_squared_distribution<double> distribution(x);
                  return distribution(generator);
            }
            int discrete_distribution(int min, int max)
            {
                  std::discrete_distribution<int> distribution(min,max);
                  return distribution(generator);
            }
            double exponential_distribution(double x)
            {
                  std::exponential_distribution<double> distribution(x);
                  return distribution(generator);
            }
            double fisher_distribution(double min, double max)
            {
                  std::fisher_f_distribution<double> distribution(min,max);
                  return distribution(generator);
            }
            double gamma_distribution(double min, double max)
            {
                  std::gamma_distribution<double> distribution(min,max);
                  return distribution(generator);
            }
            int geometric_distribution(double x)
            {
                  std::geometric_distribution<int> distribution(x);
                  return distribution(generator);
            }
            double lognormal_distribution(double min, double max)
            {
                  std::lognormal_distribution<double> distribution(min,max);
                  return distribution(generator);
            }
            int negative_binomial_distribution(int min, int max)
            {
                  std::negative_binomial_distribution<int> distribution(min,max);
                  return distribution(generator);
            }
            double normal_distribution(double a, double b)
            {
                  std::normal_distribution<double> distribution(a,b);
                  return distribution(generator);
            }
            double piecewise_constant_distribution()
            {
                  std::piecewise_constant_distribution<double> distribution;
                  return distribution(generator);
            }
            double piecewise_linear_distribution()
            {
                  std::piecewise_linear_distribution<double> distribution;
                  return distribution(generator);
            }
            int poisson_distribution(double p)
            {
                  std::poisson_distribution<int> distribution(p);
                  return distribution(generator);
            }      
      };
}      