// Script reference for counting fps & average fps in debug mode
// put the script in scripts folder

public class DeviceTypeUI : MonoBehaviour
{
    /* Assign this script to any object in the Scene to display frames per second */

    public float updateInterval = 0.5f; //How often should the number update

    float accum = 0.0f;
    float timeleft;
    float fps;
    float totalFps = 0;
    float averageFps = 0;

    int frames = 0;
    int cnt = 0;


    GUIStyle textStyle = new GUIStyle();

    // Start is called before the first frame update
    void Start()
    {
        string deviceName = SystemInfo.graphicsDeviceName;
        string deviceTypeName = SystemInfo.graphicsDeviceType.ToString();
        GetComponent<Text>().text = deviceName + "\n" + deviceTypeName;

        timeleft = updateInterval;

        textStyle.fontStyle = FontStyle.Bold;
        textStyle.normal.textColor = Color.white;

    }

    // Update is called once per frame
    void Update()
    {
        timeleft -= Time.deltaTime;
        accum += Time.timeScale / Time.deltaTime;
        ++frames;

        // Interval ended - update GUI text and start new interval
        if (timeleft <= 0.0)
        {
            // display two fractional digits (f2 format)
            fps = (accum / frames);
            timeleft = updateInterval;
            accum = 0.0f;
            frames = 0;

            totalFps += fps;
            cnt++;
            averageFps = totalFps / cnt;

            string currentFps = "Average FPS is: " + averageFps.ToString("F2");
            Debug.Log(currentFps);
        }


    }

    void OnGUI()
    {
        //Display the fps and round to 2 decimals
        GUI.Label(new Rect(5, 5, 100, 25), averageFps.ToString("F2") + " FPS", textStyle);
    }
}

