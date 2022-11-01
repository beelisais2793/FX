#include "rurataeunity.h"

#include <memory>

namespace ruratae {

namespace {

// Unique instance of the Ruratae system.
// TODO(anokta): Add multi-instrument support.
static std::unique_ptr<instrument> ruratae = nullptr;

}  // namespace

// TODO(anokta): Don't we also need at least the sample rate here?
void Initialize(int max_particles, int max_springs) {
  ruratae.reset(new instrument(max_particles, max_springs));
}

void Shutdown() { ruratae.release(); }

void Process(int length, float* output) {
  // TODO(anokta): Return center of mass here.
  ruratae->process(output, length);
}

void SetListenerPosition(vec3 position) { ruratae->set_listener(position); }

void SetGravity(vec3 gravity) { ruratae->set_gravity(gravity); }

// TODO(anokta): It's probably better to minimize the params for create calls,
// and use the corresponding setter calls instead.
int CreateParticle(vec3 position, vec3 velocity, float mass, float radius,
                   float elasticity) {
  const particle_params params(position, velocity, mass, radius, elasticity);
  return ruratae->create_particle(params);
}

void DestroyParticle(int particle_id) {
  ruratae->destroy_particle(particle_id);
}

void SetParticlePosition(int particle_id, vec3 position) {
  ruratae->set_particle_position(particle_id, position);
}

void SetParticleRadius(int particle_id, float radius) {
  ruratae->set_particle_radius(particle_id, radius);
}

void SetParticleVelocity(int particle_id, vec3 velocity) {
  ruratae->set_particle_velocity(particle_id, velocity);
}

// TODO(anokta): It's probably better to minimize the params for create calls,
// and use the corresponding setter calls instead.
int CreateSpring(int particle_a_id, int particle_b_id, float stiffness,
                 float damping, float restlength) {
  const spring_params params(particle_a_id, particle_b_id, stiffness, damping,
                             restlength);
  return ruratae->create_spring(params);
}

void DestroySpring(int spring_id) { ruratae->destroy_spring(spring_id); }

}  // namespace ruratae
