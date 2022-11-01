using UnityEngine;
using System;
using System.Collections;
using System.Runtime.InteropServices;

// Main Ruratae class that communicates with the native implementation.
public static class Ruratae {
  // Initializes the system.
  public static void InitializeSystem (int maxParticles, int maxSprings) {
    if (!initialized) {
      Initialize(maxParticles, maxSprings);
      initialized = true;
    }
  }

  // Shuts down the system.
  public static void ShutdownSystem () {
    if (initialized) {
      initialized = false;
      Shutdown();
    }
  }

  // Processes the next buffer.
  public static void Process (float[] data) {
    if (initialized) {
      Process(data.Length, data);
    }
  }

  public static void SetListenerPosition (Transform listener) {
    if (initialized) {
      SetListenerPosition(Vec3FromUnityVector(listener.position));
    }
  }

  public static void SetGravity (Vector3 gravity) {
    if (initialized) {
      SetGravity(Vec3FromUnityVector(gravity));
    }
  }

  public static int CreateParticle (Particle particle) {
    int id = -1;
    if (initialized) {
      Vec3 pos = Vec3FromUnityVector(particle.transform.position);
      Vec3 vel = Vec3FromUnityVector(particle.velocity);
      id = CreateParticle(pos, vel, particle.recipMass, particle.radius, particle.elasticity);
    }
    return id;
  }

  public static void DestroyParticle (Particle particle) {
    if (initialized) {
      DestroyParticle(particle.Id);
    }
  }

  public static void SetParticleParams (Particle particle) {
    if (initialized && particle.Id != -1) {
      SetParticlePosition(particle.Id, Vec3FromUnityVector(particle.transform.position));
      SetParticleRadius(particle.Id, particle.radius);
      SetParticlePosition(particle.Id, Vec3FromUnityVector(particle.velocity));
    }
  }

  public static int CreateSpring (Spring spring) {
    int id = -1;
    if (initialized) {
      id = CreateSpring(spring.particleA.Id, spring.particleB.Id, spring.damping, spring.stiffness, 
                        spring.restLength);
    }
    return id;
  }

  public static void DestroySpring (Spring spring) {
    if (initialized) {
      DestroySpring(spring.Id);
    }
  }

  // Simple three-dimensional vector.
  [StructLayout(LayoutKind.Sequential)]
  private struct Vec3 {
    public float x;
    public float y;
    public float z;
  };

  private static Vec3 Vec3FromUnityVector (Vector3 unityVector) {
    Vec3 vec = new Vec3();
    vec.x = unityVector.x;
    vec.y = unityVector.y;
    vec.z = unityVector.z;
    return vec;
  }

  // Denotes whether the system is initialized.
  private static bool initialized = false;

#if UNITY_IOS
  private const string pluginName = "__Internal";
#else
  private const string pluginName = "rurataeunity";
#endif

  // System handlers.
  [DllImport(pluginName)]
  private static extern void Initialize (int maxParticles, int maxSprings);

  [DllImport(pluginName)]
  private static extern void Shutdown ();

  [DllImport(pluginName)]
  private static extern void Process (int length, [In, Out]float[] output);

  [DllImport(pluginName)]
  private static extern void SetListenerPosition (Vec3 position);

  [DllImport(pluginName)]
  private static extern void SetGravity (Vec3 gravity);

  // Particle handlers.
  [DllImport(pluginName)]
  private static extern int CreateParticle (Vec3 position, Vec3 velocity, float recipMass,
                                            float radius, float restitution);

  [DllImport(pluginName)]
  private static extern void DestroyParticle (int particleId);

  [DllImport(pluginName)]
  private static extern int SetParticlePosition (int particleId, Vec3 position);

  [DllImport(pluginName)]
  private static extern int SetParticleRadius (int particleId, float radius);

  [DllImport(pluginName)]
  private static extern int SetParticleVelocity (int particleId, Vec3 velocity);

  // Spring handlers.
  [DllImport(pluginName)]
  private static extern int CreateSpring (int particleIdA, int particleIdB, float stiffness,
                                          float damping, float restlength);

  [DllImport(pluginName)]
  private static extern void DestroySpring (int springId);
}
