using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class RagdollTrigger : MonoBehaviour {

	// Use this for initialization
	void Start ()
	{
		
	}
	
	// Update is called once per frame
	void Update () {
		
	}

	void OnTriggerEnter(Collider col)
	{
		if (col.tag == "Ragdoll")
		{
			col.GetComponentInParent<Ragdoll>().SetRagdoll(true);
		
		}
	}

	void OnCollisionEnter(Collision col)
	{
		if (col.collider.tag == "Ragdoll")
		{
			col.collider.GetComponentInParent<Ragdoll>().SetRagdoll(true);
			
		}
	}
}
