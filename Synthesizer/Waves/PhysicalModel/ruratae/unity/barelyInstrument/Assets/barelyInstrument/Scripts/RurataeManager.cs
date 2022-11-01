using UnityEngine;
using System.Collections;

public class RurataeManager : MonoBehaviour {
  // Particle prefab.
  public GameObject particlePrefab;

  // Spring prefab.
  public GameObject springPrefab;

  // Listener center position.
  public Transform listener = null;

  // Maximum number of particles allowed.
  public int maxParticles = 32;

  // Maximum number of springs allowed.
  public int maxSprings = 32;

  // Gravity.
  public Vector3 gravity = Vector3.zero;

  // TODO(anokta): Refactor this to an instrument structure.
  [SerializeField]
  private AudioSource source = null;
  private float[] monoData = null;

  void Awake () {
    if(source == null) {
      source = gameObject.AddComponent<AudioSource>();
      int numFrames = AudioSettings.GetConfiguration().dspBufferSize;
      monoData = new float[numFrames];
    }
    Ruratae.InitializeSystem(maxParticles, maxSprings);
  }

  void OnDestroy () {
    Ruratae.ShutdownSystem();
    monoData = null;
  }

  void Update () {
    Ruratae.SetListenerPosition(listener);
    Ruratae.SetGravity(gravity);
  }

  void OnAudioFilterRead(float[] data, int channels) {
    Ruratae.Process(monoData);
    for(int frame = 0; frame < monoData.Length; ++frame) {
      for(int ch = 0; ch < channels; ++ch) {
        data[channels * frame + ch] = monoData[frame];
      }
    }
  }

  public Particle InstantiateParticle (Vector3 position) {
    GameObject particleObject = GameObject.Instantiate(particlePrefab, transform) as GameObject;
    Particle particle = particleObject.GetComponent<Particle>();
    particle.transform.position = position;
    return particle;
  }

  public void DestroyParticle (Particle particle) {
    GameObject.Destroy(particle.gameObject);
  }

  public Spring InstantiateSpring (Particle a, Particle b) {
    GameObject springObject = GameObject.Instantiate(springPrefab, transform) as GameObject;
    Spring spring = springObject.GetComponent<Spring>();
    spring.particleA = a;
    spring.particleB = b;
    spring.enabled = true;
    return spring;
  }

  public void DestroySpring (Spring spring) {
    GameObject.Destroy(spring.gameObject);
  }
}
