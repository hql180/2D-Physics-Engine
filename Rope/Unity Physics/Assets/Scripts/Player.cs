using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[RequireComponent (typeof(CharacterController))]
public class Player : MonoBehaviour {
    CharacterController controller = null;
    Animator animator = null;

    public float speed = 80.0f;
    public float pushPower = 2.0f;
	public float mouseSensitivity = 70f;

    public GameObject particleSpawner;

	// Use this for initialization
	void Start ()
	{
        controller = GetComponent<CharacterController>();
		Cursor.lockState = CursorLockMode.Locked;
		Cursor.visible = false;
	}
	
	// Update is called once per frame
	void Update ()
	{
        if(Input.GetMouseButtonDown(0))
        {
            RaycastHit hit;
            var ray = Camera.main.ScreenPointToRay(new Vector3(Screen.width / 2, Screen.height / 2, 0));

            if(Physics.Raycast(ray, out hit))
            {
                if(particleSpawner)
                {
                    var rot = Quaternion.FromToRotation(Vector3.up, hit.normal);

                    var spawner = Instantiate(particleSpawner, hit.point, rot);
                    spawner.transform.parent = hit.transform;
                }
            }
        }

		if (Input.GetKeyDown(KeyCode.Escape))
		{
			if (Cursor.visible == true)
			{
				Cursor.lockState = CursorLockMode.Locked;
				Cursor.visible = false;
			}
			else
			{
				Cursor.lockState = CursorLockMode.None;
				Cursor.visible = true;
			}
		}

        float vertical = Input.GetAxis("Vertical");
        float horizontal = Input.GetAxis("Horizontal");

		float mouseX = Input.GetAxis("Mouse X");
		float mouseY = Input.GetAxis("Mouse Y");
		Camera.main.transform.Rotate(new Vector3(-mouseY, 0, 0));
		
        transform.Rotate(transform.up, mouseX * mouseSensitivity * Time.deltaTime);
		
		controller.Move((transform.forward * vertical + transform.right * horizontal) * speed * Time.deltaTime);
     
	}

    void OnControllerColliderHit(ControllerColliderHit hit)
    {
        Rigidbody body = hit.collider.attachedRigidbody;
        if (body == null || body.isKinematic)
            return;

        if (hit.moveDirection.y < -0.3F)
            return;

        Vector3 pushDir = new Vector3(hit.moveDirection.x, 0, hit.moveDirection.z);
        body.velocity = pushDir * pushPower;
    }
}
