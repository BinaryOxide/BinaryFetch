#include "ExtraInfo.h"
#include <iostream>
#include <windows.h>
#include <mmdeviceapi.h>
#include <functiondiscoverykeys_devpkey.h>
#include <powrprof.h>
#include <sstream>
using namespace std;

/**
 * Sets the console text color
 * @param color - Integer representing the color code (e.g., 7=white, 10=green, 12=red)
 */
void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

/**
 * Retrieves and displays all audio devices (output and input) on the system
 * Marks active/default devices with color-coded indicators
 * @return Empty string on success, error message on failure
 */
string ExtraInfo::get_audio_devices()
{
    HRESULT hr;

    // Initialize COM library for current thread
    CoInitialize(nullptr);

    // Pointers for device enumeration
    IMMDeviceEnumerator* pEnum = nullptr;
    IMMDeviceCollection* pDevices = nullptr;

    // Create an instance of the device enumerator
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
        __uuidof(IMMDeviceEnumerator), (void**)&pEnum);
    if (FAILED(hr))
    {
        CoUninitialize();
        return "Audio Devices: Failed to enumerate.";
    }

    // ==================== OUTPUT DEVICES (Speakers/Headphones) ====================

    // Enumerate all rendering (output) audio devices, both active and disabled
    hr = pEnum->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE | DEVICE_STATE_DISABLED, &pDevices);
    if (SUCCEEDED(hr))
    {
        UINT count = 0;
        pDevices->GetCount(&count);  // Get total number of output devices

        // Get the default output device to compare later
        IMMDevice* pDefaultOut = nullptr;
        pEnum->GetDefaultAudioEndpoint(eRender, eConsole, &pDefaultOut);

        // Iterate through all output devices
        for (UINT i = 0; i < count; i++)
        {
            IMMDevice* pDevice = nullptr;
            IPropertyStore* pProps = nullptr;
            LPWSTR deviceId = nullptr;

            // Get device at index i
            pDevices->Item(i, &pDevice);

            // Open property store to read device properties
            pDevice->OpenPropertyStore(STGM_READ, &pProps);

            // Initialize and get the friendly name of the device
            PROPVARIANT name;
            PropVariantInit(&name);
            pProps->GetValue(PKEY_Device_FriendlyName, &name);

            // Get unique device ID
            pDevice->GetId(&deviceId);

            // Convert wide string to standard string
            wstring wname(name.pwszVal);
            string deviceName(wname.begin(), wname.end());

            // Check if this device is the active (default) output device
            bool isActive = false;
            if (pDefaultOut)
            {
                LPWSTR defId = nullptr;
                pDefaultOut->GetId(&defId);

                // Compare device IDs
                if (wcscmp(deviceId, defId) == 0)
                    isActive = true;

                CoTaskMemFree(defId);  // Free allocated memory
            }

            // Display device name
            cout << "  " << deviceName;

            // Mark active device in green
            if (isActive)
            {
                setColor(10); // Green
                cout << " (active)";
                setColor(7); // Reset to default color
            }
            cout << endl;

            // Clean up resources for this device
            PropVariantClear(&name);
            if (pProps) pProps->Release();
            if (pDevice) pDevice->Release();
            CoTaskMemFree(deviceId);
        }

        // Release default output device and collection
        if (pDefaultOut) pDefaultOut->Release();
        pDevices->Release();
    }

    // ==================== INPUT DEVICES (Microphones) ====================

    // Enumerate all capture (input) audio devices, both active and disabled
    hr = pEnum->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE | DEVICE_STATE_DISABLED, &pDevices);
    if (SUCCEEDED(hr))
    {
        UINT count = 0;
        pDevices->GetCount(&count);  // Get total number of input devices

        // Get the default input device to compare later
        IMMDevice* pDefaultIn = nullptr;
        pEnum->GetDefaultAudioEndpoint(eCapture, eConsole, &pDefaultIn);

        // Iterate through all input devices
        for (UINT i = 0; i < count; i++)
        {
            IMMDevice* pDevice = nullptr;
            IPropertyStore* pProps = nullptr;
            LPWSTR deviceId = nullptr;

            // Get device at index i
            pDevices->Item(i, &pDevice);

            // Open property store to read device properties
            pDevice->OpenPropertyStore(STGM_READ, &pProps);

            // Initialize and get the friendly name of the device
            PROPVARIANT name;
            PropVariantInit(&name);
            pProps->GetValue(PKEY_Device_FriendlyName, &name);

            // Get unique device ID
            pDevice->GetId(&deviceId);

            // Convert wide string to standard string
            wstring wname(name.pwszVal);
            string deviceName(wname.begin(), wname.end());

            // Check if this device is the active (default) input device
            bool isActive = false;
            if (pDefaultIn)
            {
                LPWSTR defId = nullptr;
                pDefaultIn->GetId(&defId);

                // Compare device IDs
                if (wcscmp(deviceId, defId) == 0)
                    isActive = true;

                CoTaskMemFree(defId);  // Free allocated memory
            }

            // Display device name
            cout << "  " << deviceName;

            // Mark active device with colored brackets and text
            if (isActive)
            {
                setColor(11); // Sky blue for opening bracket
                cout << "(";
                setColor(10); // Green for "active" text
                cout << "active";
                setColor(11); // Sky blue for closing bracket
                cout << ")";
                setColor(7);  // Reset to default color
            }
            cout << endl;

            // Clean up resources for this device
            PropVariantClear(&name);
            if (pProps) pProps->Release();
            if (pDevice) pDevice->Release();
            CoTaskMemFree(deviceId);
        }

        // Release default input device and collection
        if (pDefaultIn) pDefaultIn->Release();
        pDevices->Release();
    }

    // Release enumerator and uninitialize COM
    if (pEnum) pEnum->Release();
    CoUninitialize();

    return "";
}

/**
 * Retrieves and displays system power status
 * Shows whether system is on battery or AC power, battery percentage, and charging status
 * @return Empty string on success, error message on failure
 */
string ExtraInfo::get_power_status()
{
    SYSTEM_POWER_STATUS sps;

    // Get system power status information
    if (!GetSystemPowerStatus(&sps))
        return "Power Status: Unknown";

    // Check if system has a battery (BatteryFlag == 128 means no battery)
    bool hasBattery = (sps.BatteryFlag != 128);

    cout << "Power Status: ";

    // Handle desktop systems without battery
    if (!hasBattery)
    {
        setColor(11); // Sky blue
        cout << "[Wired connection]";
        setColor(7);  // Reset to default color
        cout << endl;
        return "";
    }

    // Display battery-powered status with percentage
    cout << "Battery powered ";
    setColor(11); cout << "(";           // Sky blue opening bracket
    setColor(14); cout << (int)sps.BatteryLifePercent << "%"; // Orange percentage
    setColor(11); cout << ")";           // Sky blue closing bracket
    setColor(7);  // Reset to default color

    // Check and display charging status
    if (sps.ACLineStatus == 1)  // AC power is online (charging)
    {
        cout << " ";
        setColor(10); cout << "(";       // Green opening bracket
        setColor(10); cout << "Charging"; // Green "Charging" text
        setColor(10); cout << ")";       // Green closing bracket
        setColor(7);  // Reset to default color
    }
    else  // AC power is offline (not charging)
    {
        cout << " ";
        setColor(11); cout << "(";           // Sky blue opening bracket
        setColor(12); cout << "Not Charging"; // Red "Not Charging" text
        setColor(11); cout << ")";           // Sky blue closing bracket
        setColor(7);  // Reset to default color
    }

    cout << endl;
    return "";
}

/*
================================================================================
                        END-OF-FILE DOCUMENTATION
================================================================================

FUNCTION REFERENCE:

1. SetConsoleTextAttribute(HANDLE, WORD)
   - Sets text color/attributes for console output
   - Parameters: Console handle, color/attribute code
   - Returns: TRUE on success

2. GetStdHandle(DWORD)
   - Retrieves handle to standard input/output/error device
   - STD_OUTPUT_HANDLE = standard output (console)
   - Returns: Handle to specified device

3. CoInitialize(LPVOID)
   - Initializes COM library for current thread
   - Must be called before using COM objects
   - Parameter: Reserved (use nullptr)
   - Returns: S_OK on success

4. CoCreateInstance(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*)
   - Creates single object of specified class
   - Used to create MMDeviceEnumerator instance
   - Returns: HRESULT indicating success/failure

5. EnumAudioEndpoints(EDataFlow, DWORD, IMMDeviceCollection**)
   - Enumerates audio endpoint devices
   - EDataFlow: eRender (output) or eCapture (input)
   - DWORD: Device state filter (ACTIVE, DISABLED, etc.)
   - Returns: Collection of devices

6. GetDefaultAudioEndpoint(EDataFlow, ERole, IMMDevice**)
   - Gets default audio endpoint for specified data flow
   - ERole: eConsole, eMultimedia, or eCommunications
   - Returns: Default device interface

7. GetCount(UINT*)
   - Gets number of devices in collection
   - Parameter: Pointer to receive count
   - Returns: S_OK on success

8. Item(UINT, IMMDevice**)
   - Gets device at specified index from collection
   - Returns: Device interface pointer

9. OpenPropertyStore(DWORD, IPropertyStore**)
   - Opens property store for device
   - STGM_READ: Read-only access
   - Returns: Property store interface

10. PropVariantInit(PROPVARIANT*)
    - Initializes PROPVARIANT structure
    - Must be called before using PROPVARIANT

11. GetValue(REFPROPERTYKEY, PROPVARIANT*)
    - Retrieves property value from store
    - PKEY_Device_FriendlyName: Device's display name
    - Returns: S_OK on success

12. GetId(LPWSTR*)
    - Gets unique device identifier string
    - Returns: Wide string device ID

13. wcscmp(const wchar_t*, const wchar_t*)
    - Compares two wide-character strings
    - Returns: 0 if equal, <0 if first < second, >0 if first > second

14. PropVariantClear(PROPVARIANT*)
    - Frees memory used by PROPVARIANT structure
    - Must be called after finishing with PROPVARIANT

15. Release()
    - Decrements COM object reference count
    - Frees object when count reaches zero
    - Called on all COM interfaces when done

16. CoTaskMemFree(LPVOID)
    - Frees memory allocated by COM task memory allocator
    - Used for strings allocated by GetId()

17. CoUninitialize()
    - Closes COM library for current thread
    - Must be called after CoInitialize()

18. GetSystemPowerStatus(LPSYSTEM_POWER_STATUS)
    - Retrieves system power status
    - Returns: TRUE on success, FALSE on failure

STRUCTURES USED:

- SYSTEM_POWER_STATUS: Contains power status information
  * ACLineStatus: 0=offline, 1=online, 255=unknown
  * BatteryFlag: Battery status (128=no battery)
  * BatteryLifePercent: 0-100 or 255 if unknown
  * BatteryLifeTime: Seconds of battery life remaining
  * BatteryFullLifeTime: Seconds when fully charged

- PROPVARIANT: Variant data type for property values
  * pwszVal: Wide string value for device names

COLOR CODES USED:
- 7  = Light Gray (Default)
- 10 = Light Green (Active status)
- 11 = Light Cyan/Sky Blue (Brackets)
- 12 = Light Red (Not charging)
- 14 = Yellow/Orange (Battery percentage)

================================================================================
*/