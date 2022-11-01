using UnityEngine;
using UnityEngine.EventSystems;
using System.Collections;

// Ruratae spring.
public class Spring : MonoBehaviour, IPointerClickHandler {
  public Particle particleA;

  public Particle particleB;

  public float stiffness = 3000000.0f;

  public float damping = 2000000.0f;

  public float restLength = 0.0f;

  // Unique id.
  public int Id {
    get { return id; }
  }
  private int id = -1;

  void OnEnable () {
    id = Ruratae.CreateSpring(this);
  }

  void Start() {
    if(id == -1) {
      id = Ruratae.CreateSpring(this);
    }
  }

  void OnDisable() {
    if(id != -1) {
      Ruratae.DestroySpring(this);
      id = -1;
    }
  }

  void Update () {
    if(particleA == null || particleB == null) {
      GameObject.Destroy(gameObject);
      return;
    }
    SetTransform(particleA.transform.position, particleB.transform.position);
  }

  public void SetTransform(Vector3 begin, Vector3 end) {
    Vector3 direction = end - begin;
    transform.position = 0.5f * (begin + end);
    transform.rotation = Quaternion.FromToRotation(Vector3.up, direction);
    float length = 0.5f * direction.magnitude;
    float thickness = 0.15f; // * restLength;
    transform.localScale = new Vector3(thickness, length, thickness);
  }

  // Implements |IPointerClickHandler.OnPointerClick|.
  public void OnPointerClick(PointerEventData eventData) {
    GameObject.Destroy(gameObject);
  }
}
