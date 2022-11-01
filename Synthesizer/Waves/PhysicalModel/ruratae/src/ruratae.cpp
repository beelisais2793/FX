#include "ruratae.hpp"

namespace ruratae
{
//=========================================================================
// core
//=========================================================================
instrument::instrument(int max_particles, int max_springs)
  :
  m_particle_internal(max_particles),
  m_spring_internal(max_springs)
{
  // initialize token stacks
  for (int i = max_particles - 1; i >= 0; i--)
  {
    m_particle_stack.push(i);
  }
  for (int i = max_springs - 1; i >= 0; i--)
  {
    m_spring_stack.push(i);
  }
}

instrument::~instrument()
{
  //TODO: cleanup
}

vec3 instrument::process(float* out_buffer, int num_samps)
{
  // !!! NOTE: routine is HIGH-PRIORITY. use try_lock avoid thread blocks.
  set_msg v(set_msg::INVALID_MSG);
  int queue_i = 0;
  const int max_queue_items = 50; //TODO: user-configurable?
  while (queue_i < max_queue_items)
  {
		if (!m_setter_queue.try_pop(v))
			break;
    mfn_process_queue_item(v);
    queue_i++;
  }

  //TODO: real DSP here...
  const float dt = 1.0f / 96000.0f;
  for (int i = 0; i < num_samps; i++)
  {
    mfn_process_sample(m_spring_internal, m_particle_internal, dt);
    *out_buffer++ = mfn_listento_sample(m_particle_internal, m_listener);
  }
  return vec3();
}

void instrument::drawlist()
{
  //TODO: do something render-y here
}

//=========================================================================
// global settings
//=========================================================================
vec3 instrument::get_listener() const
{
  return m_listener;
}

void instrument::set_listener(const vec3& v)
{
  set_msg msg(set_msg::INSTRUMENT_LISTENER);
  msg.val_vec3 = v;
  m_setter_queue.push(msg);
}

vec3 instrument::get_gravity() const
{
  return m_gravity;
}

void instrument::set_gravity(const vec3& v)
{
  set_msg msg(set_msg::INSTRUMENT_GRAVITY);
  msg.val_vec3 = v;
  m_setter_queue.push(msg);
}

//=========================================================================
// particles
//=========================================================================
int instrument::create_particle(const particle_params& p)
{
  int v;
  if (!m_particle_stack.pop(v))
    return -1;
  set_msg msg(set_msg::PARTICLE_CREATE);
  msg.handle = v;
  msg.val_pp = p;
  m_setter_queue.push(msg);
  return v;
}

void instrument::destroy_particle(int handle)
{
  //TODO: ensure handle is valid before processing...
  m_particle_stack.push(handle);
  set_msg msg(set_msg::PARTICLE_DESTROY);
  msg.handle = handle;
  m_setter_queue.push(msg);
}

vec3 instrument::get_particle_position(int handle)
{
  //TODO: ensure handle is valid before processing...
  return m_particle_internal[handle].p;
}

void instrument::set_particle_position(int handle, const vec3& v)
{
  //TODO: ensure handle is valid before processing...
  set_msg msg(set_msg::PARTICLE_POSITION);
  msg.handle = handle;
  msg.val_vec3 = v;
  m_setter_queue.push(msg);
}

vec3 instrument::get_particle_velocity(int handle)
{
  //TODO: ensure handle is valid before processing...
  return m_particle_internal[handle].v;
}

void instrument::set_particle_velocity(int handle, const vec3& v)
{
  //TODO: ensure handle is valid before processing...
  set_msg msg(set_msg::PARTICLE_VELOCITY);
  msg.handle = handle;
  msg.val_vec3 = v;
  m_setter_queue.push(msg);
}

float instrument::get_particle_reciprocal_mass(int handle)
{
  //TODO: ensure handle is valid before processing...
  return m_particle_internal[handle].rm;
}

void instrument::set_particle_reciprocal_mass(int handle, float v)
{
  //TODO: ensure handle is valid before processing...
  set_msg msg(set_msg::PARTICLE_RECIPMASS);
  msg.handle = handle;
  msg.val_f32 = v;
  m_setter_queue.push(msg);
}

float instrument::get_particle_radius(int handle)
{
  //TODO: ensure handle is valid before processing...
  return m_particle_internal[handle].rad;
}

void instrument::set_particle_radius(int handle, float v)
{
  //TODO: ensure handle is valid before processing...
  set_msg msg(set_msg::PARTICLE_RADIUS);
  msg.handle = handle;
  msg.val_f32 = v;
  m_setter_queue.push(msg);
}

float instrument::get_particle_elasticity(int handle)
{
  //TODO: ensure handle is valid before processing...
  return m_particle_internal[handle].el;
}

void instrument::set_particle_elasticity(int handle, float v)
{
  //TODO: ensure handle is valid before processing...
  set_msg msg(set_msg::PARTICLE_ELASTICITY);
  msg.handle = handle;
  msg.val_f32 = v;
  m_setter_queue.push(msg);
}

//=========================================================================
// springs
//=========================================================================
int instrument::create_spring(const spring_params& s)
{
  int v;
  if (!m_spring_stack.pop(v))
    return -1;
  set_msg msg(set_msg::SPRING_CREATE);
  msg.handle = v;
  msg.val_sp = s;
  m_setter_queue.push(msg);
  return v;
}

void instrument::destroy_spring(int handle)
{
  //TODO: ensure handle is valid before processing...
  set_msg msg(set_msg::SPRING_DESTROY);
  msg.handle = handle;
  m_setter_queue.push(msg);
}

int instrument::get_spring_particle_a(int handle)
{
  //TODO: ensure handle is valid before processing...
  return m_spring_internal[handle].a_handle;
}

void instrument::set_spring_particle_a(int handle, int particle_handle)
{
  //TODO: ensure handle is valid before processing...
  set_msg msg(set_msg::SPRING_PARTICLEA);
  msg.handle = handle;
  msg.val_i32 = particle_handle;
  m_setter_queue.push(msg);
}

int instrument::get_spring_particle_b(int handle)
{
  //TODO: ensure handle is valid before processing...
  return m_spring_internal[handle].b_handle;
}

void instrument::set_spring_particle_b(int handle, int particle_handle)
{
  //TODO: ensure handle is valid before processing...
  set_msg msg(set_msg::SPRING_PARTICLEB);
  msg.handle = handle;
  msg.val_i32 = particle_handle;
  m_setter_queue.push(msg);
}

float instrument::get_spring_stiffness(int handle)
{
  //TODO: ensure handle is valid before processing...
  return m_spring_internal[handle].k;
}

void instrument::set_spring_stiffness(int handle, float v)
{
  //TODO: ensure handle is valid before processing...
  set_msg msg(set_msg::SPRING_STIFFNESS);
  msg.handle = handle;
  msg.val_f32 = v;
  m_setter_queue.push(msg);
}

float instrument::get_spring_damping(int handle)
{
  //TODO: ensure handle is valid before processing...
  return m_spring_internal[handle].c;
}

void instrument::set_spring_damping(int handle, float v)
{
  //TODO: ensure handle is valid before processing...
  set_msg msg(set_msg::SPRING_DAMPING);
  msg.handle = handle;
  msg.val_f32 = v;
  m_setter_queue.push(msg);
}

float instrument::get_spring_restlength(int handle)
{
  //TODO: ensure handle is valid before processing...
  return m_spring_internal[handle].L;
}

void instrument::set_spring_restlength(int handle, float v)
{
  //TODO: ensure handle is valid before processing...
  set_msg msg(set_msg::SPRING_RESTLENGTH);
  msg.handle = handle;
  msg.val_f32 = v;
  m_setter_queue.push(msg);
}

//=========================================================================
// private methods
//=========================================================================
void instrument::mfn_process_spring(const spring& s)
{
  particle& a = *(s.a);
  particle& b = *(s.b);
  vec3 dp(b.p.x - a.p.x, b.p.y - a.p.y, b.p.z - a.p.z);
  vec3 dv(b.v.x - a.v.x, b.v.y - a.v.y, b.v.z - a.v.z);
  float recip_len =
    1.0f / sqrtf(dp.x * dp.x + dp.y * dp.y + dp.z * dp.z);
  vec3 f(-s.k * dp.x + s.k * s.L * dp.x * recip_len - s.c * dv.x,
    -s.k * dp.y + s.k * s.L * dp.y * recip_len - s.c * dv.y,
    -s.k * dp.z + s.k * s.L * dp.z * recip_len - s.c * dv.z);
  a.f.x -= f.x;
  a.f.y -= f.y;
  a.f.z -= f.z;
  b.f.x += f.x;
  b.f.y += f.y;
  b.f.z += f.z;
}

void instrument::mfn_process_particle(particle& p, float dt)
{
  const float drag = 1.0f;
  p.v.x += p.f.x * dt * dt * p.rm * drag;
  p.v.y += p.f.y * dt * dt * p.rm * drag;
  p.v.z += p.f.z * dt * dt * p.rm * drag;
  p.p.x += p.v.x;
  p.p.y += p.v.y;
  p.p.z += p.v.z;
  p.f.x = 0.0f;
  p.f.y = 0.0f;
  p.f.z = 0.0f;
}

void instrument::mfn_process_sample(
  std::vector<spring>& springs,
  std::vector<particle>& particles, float dt)
{
  for (int i = 0; i < springs.size(); i++)
  {
    if (springs[i].enabled)
      mfn_process_spring(springs[i]);
  }
  for (int i = 0; i < particles.size(); i++)
  {
    if (particles[i].enabled)
      mfn_process_particle(particles[i], dt);
  }
}

float instrument::mfn_listento_sample(
  std::vector<particle>& particles, const vec3& listener)
{
  float output = 0.0f;
  for (int i = 0; i < particles.size(); i++)
  {
		if (particles[i].enabled && particles[i].rm > 0.0f)
		{
			vec3 dp(listener.x - particles[i].p.x,
				listener.y - particles[i].p.y,
				listener.z - particles[i].p.z);
			float recip_sqr_len =
				1.0f / (dp.x * dp.x + dp.y * dp.y + dp.z * dp.z);
			float pressure =
				(dp.x * particles[i].v.x +
					dp.y * particles[i].v.y +
					dp.z * particles[i].v.z) * recip_sqr_len;
			output += pressure;
		}
  }
  return output;
}

void instrument::mfn_process_queue_item(const set_msg& msg)
{
  if (msg.type == set_msg::INSTRUMENT_LISTENER)
  {
    m_listener = msg.val_vec3;
  }
  else if (msg.type == set_msg::INSTRUMENT_GRAVITY)
  {
    m_gravity = msg.val_vec3;
  }
  else if (msg.type == set_msg::PARTICLE_CREATE)
  {
    particle p;
    p.enabled = true;
    p.p = msg.val_pp.position;
    p.v = msg.val_pp.velocity;
    p.rm = msg.val_pp.reciprocal_mass;
    p.rad = msg.val_pp.radius;
    p.el = msg.val_pp.elasticity;
    m_particle_internal[msg.handle] = p;
  }
  else if (msg.type == set_msg::PARTICLE_DESTROY)
  {
    m_particle_internal[msg.handle].enabled = false;
  }
  else if (msg.type == set_msg::PARTICLE_POSITION)
  {
    m_particle_internal[msg.handle].p = msg.val_vec3;
  }
  else if (msg.type == set_msg::PARTICLE_VELOCITY)
  {
    m_particle_internal[msg.handle].v = msg.val_vec3;
  }
  else if (msg.type == set_msg::PARTICLE_RECIPMASS)
  {
    m_particle_internal[msg.handle].rm = msg.val_f32;
  }
  else if (msg.type == set_msg::PARTICLE_RADIUS)
  {
    m_particle_internal[msg.handle].rad = msg.val_f32;
  }
  else if (msg.type == set_msg::PARTICLE_ELASTICITY)
  {
    m_particle_internal[msg.handle].el = msg.val_f32;
  }
  else if (msg.type == set_msg::SPRING_CREATE)
  {
    spring s;
    s.enabled = true;
    s.a = &m_particle_internal[msg.val_sp.particle_a];
    s.b = &m_particle_internal[msg.val_sp.particle_b];
    s.k = msg.val_sp.stiffness;
    s.c = msg.val_sp.damping;
    s.L = msg.val_sp.restlength;
    s.a_handle = msg.val_sp.particle_a;
    s.b_handle = msg.val_sp.particle_b;
    m_spring_internal[msg.handle] = s;
  }
  else if (msg.type == set_msg::SPRING_DESTROY)
  {
    m_spring_internal[msg.handle].enabled = false;
  }
  else if (msg.type == set_msg::SPRING_PARTICLEA)
  {
    m_spring_internal[msg.handle].a = &m_particle_internal[msg.val_i32];
    m_spring_internal[msg.handle].a_handle = msg.val_i32;
  }
  else if (msg.type == set_msg::SPRING_PARTICLEB)
  {
    m_spring_internal[msg.handle].b = &m_particle_internal[msg.val_i32];
    m_spring_internal[msg.handle].b_handle = msg.val_i32;
  }
  else if (msg.type == set_msg::SPRING_STIFFNESS)
  {
    m_spring_internal[msg.handle].k = msg.val_f32;
  }
  else if (msg.type == set_msg::SPRING_DAMPING)
  {
    m_spring_internal[msg.handle].c = msg.val_f32;
  }
  else if (msg.type == set_msg::SPRING_RESTLENGTH)
  {
    m_spring_internal[msg.handle].L = msg.val_f32;
  }
  else if (msg.type == set_msg::INVALID_MSG)
  {
    //TODO: throw an error?
  }
}
}
