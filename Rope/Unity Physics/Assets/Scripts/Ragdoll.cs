using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Ragdoll: MonoBehaviour
{
	Rigidbody[] bodyParts;

	// Use this for initialization
	void Start ()
	{
		bodyParts = GetComponentsInChildren<Rigidbody>();
		foreach (var rb in bodyParts)
		{
			rb.isKinematic = true;
			//rb.gameObject.AddComponent<RagdollTrigger>();
		}
	}
	
	// Update is called once per frame
	void Update ()
	{
	}

	public void SetRagdoll(bool set)
	{
		foreach (var rb in bodyParts)
		{
			rb.isKinematic = !set;
		}

		GetComponent<Animator>().enabled = !set;
	}

	void OnTriggerEnter(Collider col)
	{
		if (col.tag == "RagdollTrigger")
		{
			SetRagdoll(true);
		}
	}

	void OnCollisionEnter(Collision col)
	{
		if (col.transform.tag == "RagdollTrigger")
		{
			SetRagdoll(true);
		}
	}
}
