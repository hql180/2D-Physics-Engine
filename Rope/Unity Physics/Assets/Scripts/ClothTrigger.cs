using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ClothTrigger : MonoBehaviour {

	public GameObject ClothSpawner;

	// Use this for initialization
	void Start ()
	{
		
	}
	
	// Update is called once per frame
	void Update ()
	{
		
	}

	void OnTriggerEnter(Collider col)
	{
		if (ClothSpawner)
		{
			foreach (Rigidbody rb in ClothSpawner.GetComponentsInChildren<Rigidbody>())
			{
				rb.isKinematic = false;
			}
		}
	}

	void OnCollisionEnter(Collision col)
	{
		if (ClothSpawner)
		{
			foreach (Rigidbody rb in ClothSpawner.GetComponentsInChildren<Rigidbody>())
			{
				rb.isKinematic = false;
			}
		}
	}
}
