using UnityEngine;
using UnityEngine.EventSystems;
using System.Collections;

public class InputManager : MonoBehaviour {
  public Camera mainCamera;
  public RurataeManager rurataeManager;

  private enum InputMode {
    Particle,
    Spring,
    None
  }

  private InputMode mode = InputMode.None;

  // THIS IS ULTRA HACKY, REMOVE ASAP!
  private Spring drawSpring;
  private Particle selectedParticle = null;

  void Awake () {
    drawSpring = 
      ((GameObject)GameObject.Instantiate(rurataeManager.springPrefab)).GetComponent<Spring>();
    drawSpring.enabled = false;
    drawSpring.SetTransform(Vector3.zero, Vector3.zero);
  }

  void Update () {
    bool objectSelected = EventSystem.current.IsPointerOverGameObject();
    bool ctrlPressed = Input.GetKey(KeyCode.LeftControl);
    if (Input.GetMouseButtonDown(0)) {
      if (ctrlPressed && objectSelected) {
        selectedParticle = GetSelectedParticle(Input.mousePosition);
        if (selectedParticle != null) {
          mode = InputMode.Spring;
          drawSpring.particleA = selectedParticle;
        }
      } else if (!objectSelected && !ctrlPressed) {
        // Instantiate new particle.
        mode = InputMode.Particle;
        selectedParticle = rurataeManager.InstantiateParticle(Vector3.zero);
      } else {
        mode = InputMode.None;
      }
    }
    switch (mode) {
    case InputMode.Particle:
      selectedParticle.transform.position = WorldFromScreenPosition(Input.mousePosition);
      if (Input.GetMouseButtonUp(0)) {
        mode = InputMode.None;
        selectedParticle = null;
      }
      break;
    case InputMode.Spring:
      Vector3 position = WorldFromScreenPosition(Input.mousePosition);
      drawSpring.SetTransform(drawSpring.particleA.transform.position, position);
      if (Input.GetMouseButtonUp(0)) {
        mode = InputMode.None;
        selectedParticle = GetSelectedParticle(Input.mousePosition);
        if (selectedParticle != null) {
          // Instantiate new spring.
          drawSpring.particleB = selectedParticle;
          rurataeManager.InstantiateSpring(drawSpring.particleA, drawSpring.particleB);
        }
        drawSpring.SetTransform(Vector3.zero, Vector3.zero);
      }
      break;
    }
  }

  private Vector3 WorldFromScreenPosition (Vector3 screenPosition) {
    screenPosition.z = -mainCamera.transform.position.z;
    return mainCamera.ScreenToWorldPoint(screenPosition);
  }

  private Particle GetSelectedParticle (Vector3 screenPosition) {
    Ray ray = mainCamera.ScreenPointToRay(screenPosition);
    RaycastHit hit;
    Physics.Raycast(ray, out hit);
    return hit.transform.GetComponent<Particle>();
  }
}
