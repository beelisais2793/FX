#ifndef RURATAE_H__
#define RURATAE_H__

#include "util.hpp"

namespace ruratae
{
struct vec3
{
  float x, y, z;
  vec3(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f)
    : x(_x), y(_y), z(_z) {}
};

struct particle_params
{
  vec3 position;
  vec3 velocity;
  float reciprocal_mass;
  float radius;
  float elasticity;

	particle_params(
		vec3 _position = vec3(0),
		vec3 _velocity = vec3(0),
		float _reciprocal_mass = 0.0f,
		float _radius = 0.0f,
		float _elasticity = 0.0f) :
		position(_position),
		velocity(_velocity),
		reciprocal_mass(_reciprocal_mass),
		radius(_radius),
		elasticity(_elasticity)
	{}
};

struct spring_params
{
  int particle_a;
  int particle_b;
  float stiffness;
  float damping;
  float restlength;

	spring_params(
		int _particle_a = 0, 
		int _particle_b = 0, 
		float _stiffness = 0.0f, 
		float _damping = 0.0f, 
		float _restlength = 0.0f) :
		particle_a(_particle_a),
		particle_b(_particle_b),
		stiffness(_stiffness),
		damping(_damping),
		restlength(_restlength)
	{}
};

class instrument
{
public:
  instrument(int max_particles, int max_springs);
  ~instrument();

  vec3 process(float* out_buffer, int num_samps);
  void drawlist();

  //=========================================================================
  // global settings
  //=========================================================================
  vec3 get_listener() const;
  void set_listener(const vec3& v);

  vec3 get_gravity() const;
  void set_gravity(const vec3& v);

  //=========================================================================
  // particles
  //=========================================================================
  int create_particle(const particle_params& p);
  void destroy_particle(int handle);

  vec3 get_particle_position(int handle);
  void set_particle_position(int handle, const vec3& v);

  vec3 get_particle_velocity(int handle);
  void set_particle_velocity(int handle, const vec3& v);

  float get_particle_reciprocal_mass(int handle);
  void set_particle_reciprocal_mass(int handle, float v);

  float get_particle_radius(int handle);
  void set_particle_radius(int handle, float v);

  float get_particle_elasticity(int handle);
  void set_particle_elasticity(int handle, float v);

  //=========================================================================
  // springs
  //=========================================================================
  int create_spring(const spring_params& s);
  void destroy_spring(int handle);

  int get_spring_particle_a(int handle);
  void set_spring_particle_a(int handle, int particle_handle);

  int get_spring_particle_b(int handle);
  void set_spring_particle_b(int handle, int particle_handle);

  float get_spring_stiffness(int handle);
  void set_spring_stiffness(int handle, float v);

  float get_spring_damping(int handle);
  void set_spring_damping(int handle, float v);

  float get_spring_restlength(int handle);
  void set_spring_restlength(int handle, float v);

private:
  struct particle
  {
    bool enabled;
    vec3 p;
    vec3 v;
    vec3 f;
    float rm;
    float rad;
    float el;
  };

  struct spring
  {
    bool enabled;
    particle* a;
    particle* b;
    float k;
    float c;
    float L;
    int a_handle;
    int b_handle;
  };

  struct set_msg
  {
    enum _setmsgtype
    {
      INSTRUMENT_LISTENER,
      INSTRUMENT_GRAVITY,
      PARTICLE_CREATE,
      PARTICLE_DESTROY,
      PARTICLE_POSITION,
      PARTICLE_VELOCITY,
      PARTICLE_RECIPMASS,
      PARTICLE_RADIUS,
      PARTICLE_ELASTICITY,
      SPRING_CREATE,
      SPRING_DESTROY,
      SPRING_PARTICLEA,
      SPRING_PARTICLEB,
      SPRING_STIFFNESS,
      SPRING_DAMPING,
      SPRING_RESTLENGTH,
      INVALID_MSG
    };
    int   type;
    int   handle;
    union
    {
      int             val_i32;
      float           val_f32;
      vec3            val_vec3;
      particle_params val_pp;
      spring_params   val_sp;
    };
    set_msg(int _type) : type(_type) {}
		set_msg(const set_msg& v)
		{
			memcpy(this, &v, sizeof(set_msg));
		}
		set_msg& operator= (const set_msg& v)
		{
			memcpy(this, &v, sizeof(set_msg));
			return *this;
		}
  };
  void mfn_process_spring(const spring& s);
  void mfn_process_particle(particle& p, float dt);
  void mfn_process_sample(std::vector<spring>& springs,
    std::vector<particle>& particles, float dt);
  float mfn_listento_sample(std::vector<particle>& particles,
    const vec3& listener);
  void mfn_process_queue_item(const set_msg& msg);
  threadsafe_queue<set_msg> m_setter_queue;
  std::vector<particle> m_particle_internal;
  threadsafe_stack<int> m_particle_stack;
  std::vector<spring>   m_spring_internal;
  threadsafe_stack<int> m_spring_stack;
  vec3 m_listener;
  vec3 m_gravity;
};
};

#endif /* RURATAE_H__ */