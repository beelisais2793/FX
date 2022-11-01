using UnityEngine;
using UnityEngine.EventSystems;
using System.Collections;

// Ruratae particle.
public class Particle : MonoBehaviour, IPointerDownHandler, IPointerUpHandler, IDragHandler {
  public Vector3 velocity = Vector3.zero;

  public float recipMass = 0.0f;

  public float radius = 0.0f;

  public float elasticity = 0.0f;

  // Unique id.
  public int Id {
    get { return id; }
  }

  private int id = -1;

  private Vector3 pressOffset;

  void OnEnable () {
    id = Ruratae.CreateParticle(this);
  }

  void Start () {
    if (id == -1) {
      id = Ruratae.CreateParticle(this);
    }
  }

  void OnDisable () {
    if (id != -1) {
      Ruratae.DestroyParticle(this);
      id = -1;
    }
  }

  void Update () {
    // Update scale.
    //transform.localScale = Vector3.Lerp(transform.localScale, radius * Vector3.one, 
    //                                    4 * Time.deltaTime);
    transform.GetComponent<Renderer>().material.color = (0.5f + recipMass) * Color.white;
    if(Input.GetMouseButton(0)) {
      // Update parameters.
      // TODO(anokta): Handle this properly to get the position from ruratae otherwise.
      Ruratae.SetParticleParams(this);
    }
  }

  // Implements |IPointerDownHandler.OnPointerDown|.
  public void OnPointerDown (PointerEventData eventData) {
    Vector3 position =  WorldFromScreenPosition(eventData.pressEventCamera, eventData.pressPosition);
    pressOffset = transform.position - position;
  }

  // Implements |IPointerUpHandler.OnPointerUp|.
  public void OnPointerUp (PointerEventData eventData) {
    if(Input.GetMouseButtonUp(1)) {
      recipMass = 1.0f - recipMass;
      velocity = recipMass * Vector3.up;
      Ruratae.SetParticleParams(this);
    } else if (!eventData.dragging) {
      GameObject.Destroy(gameObject);
    }
  }

  // Implements |IDragHandler.OnDrag|.
  public void OnDrag (PointerEventData eventData) {
    if (Input.GetKey(KeyCode.LeftControl) || recipMass == 0.0f) {
      return;
    }
    Vector3 position = WorldFromScreenPosition(eventData.pressEventCamera, eventData.position);
    transform.position = position + pressOffset;
  }

  private Vector3 WorldFromScreenPosition (Camera camera, Vector3 screenPosition) {
    screenPosition.z = -camera.transform.position.z;
    return camera.ScreenToWorldPoint(screenPosition);
  }
}
