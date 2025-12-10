// main.cpp (AsciiArt separated into header and implementation files)

#include <iostream>
#include <iomanip>        // Formatting utilities (setw, precision)
#include <vector>
#include <functional>
#include <sstream>        // For string stream operations
#include <fstream>
#include <string>

// ASCII Art functionality
#include "AsciiArt.h"

// ------------------ Full System Info Modules ------------------
#include "OSInfo.h"             // OS name, version, build info
#include "CPUInfo.h"            // CPU model, threads, cores, base/boost clocks
#include "MemoryInfo.h"         // RAM capacity, usage, speed, type
#include "GPUInfo.h"            // GPU model and basic information
#include "StorageInfo.h"        // Disk drives, partitions, used/free space
#include "NetworkInfo.h"        // Active network adapters, IP, speeds
#include "DetailedGPUInfo.h"    // Deep GPU details: VRAM usage, clocks, temps
#include "PerformanceInfo.h"    // CPU load, RAM load, GPU usage
#include "UserInfo.h"           // Username, PC name, domain
#include "SystemInfo.h"         // Motherboard, BIOS, system manufacturer
#include "DisplayInfo.h"        // Monitor resolution, refresh rate, scaling
#include "ExtraInfo.h"          // Additional misc system data


// ------------------ Compact Mode Output Modules ------------------
#include "CompactAudio.h"       // Audio device summary for compact mode
#include "CompactOS.h"          // Lightweight OS summary
#include "CompactCPU.h"         // Lightweight CPU summary
#include "CompactMemory.h"      // Lightweight RAM summary
#include "CompactScreen.h"      // Lightweight screen resolution summary
#include "CompactSystem.h"      // Lightweight motherboard/system summary
#include "CompactGPU.h"         // Lightweight GPU summary
#include "CompactPerformance.h" // Lightweight performance stats
#include "CompactUser.h"        // Lightweight user info
#include "CompactNetwork.h"     // Lightweight network info
#include "compact_disk_info.h"  // Lightweight storage/disk info (compact mode)

using namespace std;

// ------------------ main (modified to stream output) ------------------

int main() {

    // Initialize ASCII Art
    AsciiArt art;
    if (!art.loadFromFile("AsciiArt.txt")) {
        // fallback: a small placeholder art (keeps indentation)
        std::ofstream tmp("AsciiArt.txt"); // optional: create a default file
        tmp << "BinaryFetch\n";
        tmp.close();
        art.loadFromFile("AsciiArt.txt");
        std::cout << "Note: ASCII art not loaded from file; using placeholder.\n";
    }

    // Create LivePrinter
    LivePrinter lp(art);

    OSInfo os;
    CPUInfo cpu;
    MemoryInfo ram;
    GPUInfo obj_gpu;
    DetailedGPUInfo detailed_gpu_info;
    StorageInfo storage;
    NetworkInfo net;
    UserInfo user;
    PerformanceInfo perf;
    DisplayInfo display;
    ExtraInfo extra;
    SystemInfo sys;

    CompactAudio c_audio;
    CompactOS c_os;
    CompactCPU c_cpu;
    CompactScreen c_screen;
    CompactMemory c_memory;
    CompactSystem c_system;
    CompactGPU c_gpu;
    CompactPerformance c_perf;
    CompactUser c_user;
    CompactNetwork c_net;
    DiskInfo disk;

    // ---------------- Now stream info line-by-line ----------------

    // Header
    lp.push(""); // keep art line 0 + blank info (or you can push a title)
    lp.push("_>> BinaryFetch____________________________________________________");

    // Minimal OS
    {
        std::ostringstream ss;
        ss << "[OS]  -> " << c_os.getOSName()
            << c_os.getOSBuild()
            << " (" << c_os.getArchitecture() << ")"
            << " (uptime: " << c_os.getUptime() << ")";
        lp.push(ss.str());
    }

    // Minimal CPU
    {
        std::ostringstream ss;
        ss << "[CPU] -> " << c_cpu.getCPUName() << " ("
            << c_cpu.getCPUCores() << "C/"
            << c_cpu.getCPUThreads() << "T)"
            << std::fixed << std::setprecision(2)
            << " @ " << c_cpu.getClockSpeed() << " GHz ";
        lp.push(ss.str());
    }

    // Displays
    {
        auto screens = c_screen.get_screens();
        int idx = 1;
        if (screens.empty()) {
            lp.push("[Display] -> No displays detected");
        }
        else {
            for (const auto& s : screens) {
                std::ostringstream ss;
                ss << "[Display " << idx++ << "] -> "
                    << s.brand_name << " (" << s.resolution << ") @"
                    << s.refresh_rate << "Hz";
                lp.push(ss.str());
            }
        }
    }

    // Memory minimal
    {
        std::ostringstream ss;
        ss << "[Memory] -> " << "(total: " << c_memory.get_total_memory() << " GB)"
            << " (free: " << c_memory.get_free_memory() << " GB)"
            << " ( " << c_memory.get_used_memory_percent() << "% ) ";
        lp.push(ss.str());
    }

    // Audio
    {
        std::ostringstream ss1, ss2;
        ss1 << "[Audio Input] -> " << c_audio.active_audio_input() << c_audio.active_audio_input_status();
        ss2 << "[Audio Output] -> " << c_audio.active_audio_output() << c_audio.active_audio_output_status();
        lp.push(ss1.str());
        lp.push(ss2.str());
    }

    // BIOS & Motherboard (compact) - safe concatenation via ostringstream
    {
        std::ostringstream ss1;
        ss1 << "[BIOS] -> " << c_system.getBIOSInfo();
        lp.push(ss1.str());
        std::ostringstream ss2;
        ss2 << "[Motherboard] -> " << c_system.getMotherboardInfo();
        lp.push(ss2.str());
    }

    // GPU minimal
    {
        std::ostringstream ss;
        ss << "[GPU] -> " << c_gpu.getGPUName()
            << " (" << c_gpu.getGPUUsagePercent() << "%)"
            << " (" << c_gpu.getVRAMGB() << " GB)"
            << " (@" << c_gpu.getGPUFrequency() << ") ";
        lp.push(ss.str());
    }

    // Minimal Performance
    {
        std::ostringstream ss;
        ss << "[Performance] -> "
            << "(CPU: " << c_perf.getCPUUsage() << "%) "
            << "(GPU: " << c_perf.getGPUUsage() << "%) "
            << "(RAM: " << c_perf.getRAMUsage() << "%) "
            << "(Disk: " << c_perf.getDiskUsage() << "%) ";
        lp.push(ss.str());
    }

    // User
    {
        std::ostringstream ss;
        ss << "[User] -> @" << c_user.getUsername()
            << " -> (Domain: " << c_user.getDomain() << ")"
            << " -> (Type: " << c_user.isAdmin() << ")";
        lp.push(ss.str());
    }

    // Network minimal
    {
        std::ostringstream ss;
        ss << "[network] -> " << "(Name: " << c_net.get_network_name()
            << ") (Type: " << c_net.get_network_type()
            << ") (ip: " << c_net.get_network_ip() << ") ";
        lp.push(ss.str());
    }

    // Disk usage (compact)
    {
        auto disks = disk.getAllDiskUsage();
        std::ostringstream ss;
        ss << "[Disk] -> ";
        for (const auto& d : disks) {
            ss << "(" << d.first[0] << ": "
                << std::fixed << std::setprecision(1)
                << d.second << "%) ";
        }
        lp.push(ss.str());

        // capacities
        auto caps = disk.getDiskCapacity();
        std::ostringstream sc;
        sc << "[Disk Cap] -> ";
        for (const auto& c : caps) sc << "(" << c.first[0] << "-" << c.second << "GB)";
        lp.push(sc.str());
    }

    // Full detailed section (Memory Info)
    {
        lp.push(""); // blank line
        lp.push("---------------Memory Info--------------");
        {
            std::ostringstream ss;
            ss << "(Total: " << ram.getTotal() << " GB) "
                << "(Free: " << ram.getFree() << " GB) "
                << "(Used: " << ram.getUsedPercentage() << "%)";
            lp.push(ss.str());
        }

        const auto& modules = ram.getModules();
        for (size_t i = 0; i < modules.size(); ++i) {
            // --- Zero-pad capacity ---
            std::string cap = modules[i].capacity;   // e.g. "8GB"
            int num = 0;
            try { num = std::stoi(cap); }
            catch (...) { num = 0; }
            std::ostringstream capOut;
            capOut << std::setw(2) << std::setfill('0') << num << "GB";

            std::ostringstream ss;
            ss << "Memory " << i << ": "
                << "(Used: " << ram.getUsedPercentage() << "%) "
                << capOut.str() << " "
                << modules[i].type << " "
                << modules[i].speed;
            lp.push(ss.str());
        }
    }

    // Storage Info (detailed)
    {
        const auto& all_disks = storage.get_all_storage_info();
        if (all_disks.empty()) {
            lp.push("--- Storage Info ---");
            lp.push("No drives detected.");
        }
        else {
            cout << endl;
            lp.push("------------------------- STORAGE SUMMARY --------------------------");
            for (const auto& d : all_disks) {
                auto fmt_storage = [](const std::string& s) {
                    std::ostringstream oss;
                    double v = 0.0;
                    try { v = stod(s); }
                    catch (...) { v = 0.0; }
                    oss << std::fixed << std::setprecision(2)
                        << std::setw(7) << std::right << std::setfill(' ')
                        << v;
                    return oss.str();
                    };

                std::ostringstream ss;
                ss << d.storage_type << " " << d.drive_letter
                    << " [ (Used) " << fmt_storage(d.used_space)
                    << " GiB / " << fmt_storage(d.total_space)
                    << " GiB " << d.used_percentage
                    << " - " << d.file_system << " "
                    << (d.is_external ? "Ext ]" : "Int ]");
                lp.push(ss.str());
            }

            lp.push("");
            lp.push("-------------------- DISK PERFORMANCE & DETAILS --------------------");

            for (const auto& d : all_disks) {
                auto fmt_speed = [](const std::string& s) {
                    std::ostringstream tmp;
                    double v = 0.0;
                    try { v = stod(s); }
                    catch (...) { v = 0.0; }
                    tmp << std::fixed << std::setprecision(2) << v;
                    std::string val = tmp.str();
                    int padding = 7 - (int)val.size();
                    if (padding < 0) padding = 0;
                    return std::string(padding, ' ') + val;
                    };

                std::ostringstream ss;
                ss << d.drive_letter << " [ Read: "
                    << fmt_speed(d.read_speed)
                    << " MB/s | Write: "
                    << fmt_speed(d.write_speed)
                    << " MB/s | " << d.serial_number
                    << (d.is_external ? " Ext ]" : " Int ]");
                lp.push(ss.str());
            }

            lp.push("");
            lp.push("----------------- DISK PERFORMANCE & DETAILS (Predicted) ---------------");

            for (const auto& d : all_disks) {
                auto fmt_speed = [](const std::string& s) {
                    std::ostringstream tmp;
                    double v = 0.0;
                    try { v = stod(s); }
                    catch (...) { v = 0.0; }
                    tmp << std::fixed << std::setprecision(2) << v;
                    std::string val = tmp.str();
                    int padding = 7 - (int)val.size();
                    if (padding < 0) padding = 0;
                    return std::string(padding, ' ') + val;
                    };

                std::ostringstream ss;
                ss << d.drive_letter << " [ Read: ("
                    << fmt_speed(d.predicted_read_speed)
                    << " MB/s) | Write: ("
                    << fmt_speed(d.predicted_write_speed)
                    << " MB/s) | " << d.serial_number
                    << (d.is_external ? " Ext ]" : " Int ]");
                lp.push(ss.str());
            }
        }
    }

    // Network (Compact + Extra)
    {

        cout << endl;
        lp.push("--- Network Info (Compact + Extra) ---");
        {
            std::ostringstream ss;
            ss << "Network Name: " << c_net.get_network_name(); lp.push(ss.str());
        }
        {
            std::ostringstream ss;
            ss << "Network Type: " << c_net.get_network_type(); lp.push(ss.str());
        }
        {
            std::ostringstream ss;
            ss << "IP (compact): " << c_net.get_network_ip(); lp.push(ss.str());
        }



    }

    // Audio & Power
    {
        cout << endl;
        lp.push("--- Audio & Power Info ---");

        // Use full audio (ExtraInfo) - it prints directly
        ExtraInfo audio;

        // Redirect cout to a stringstream temporarily
        std::ostringstream oss;
        std::streambuf* oldCout = std::cout.rdbuf(oss.rdbuf());

        audio.get_audio_devices(); // prints to oss

        // Restore cout
        std::cout.rdbuf(oldCout);

        // Push captured lines to LivePrinter
        std::istringstream iss(oss.str());
        std::string line;
        while (std::getline(iss, line)) {
            lp.push(line);
        }

        // Power info (ExtraInfo already prints directly in your class)
        std::ostringstream ossPower;
        oldCout = std::cout.rdbuf(ossPower.rdbuf());

        audio.get_power_status();

        std::cout.rdbuf(oldCout);

        std::istringstream issPower(ossPower.str());
        while (std::getline(issPower, line)) {
            lp.push(line);
        }
    }



    // OS Info
    {
        cout << endl;
        lp.push("--- OS Info ---");
        {
            std::ostringstream ss;
            ss << "Name: " << os.GetOSName(); lp.push(ss.str());
        }
        {
            std::ostringstream ss;
            ss << "Version: " << os.GetOSVersion(); lp.push(ss.str());
        }
        {
            std::ostringstream ss;
            ss << "Architecture: " << os.GetOSArchitecture(); lp.push(ss.str());
        }
        {
            std::ostringstream ss;
            ss << "Kernel: " << os.get_os_kernel_info(); lp.push(ss.str());
        }
        {
            std::ostringstream ss;
            ss << "Uptime: " << os.get_os_uptime(); lp.push(ss.str());
        }
        {
            std::ostringstream ss;
            ss << "Install Date: " << os.get_os_install_date(); lp.push(ss.str());
        }
        {
            std::ostringstream ss;
            ss << "Serial: " << os.get_os_serial_number(); lp.push(ss.str());
        }
    }

    // CPU Info
    {
        cout << endl;
        lp.push("--- CPU Info ---");
        {
            std::ostringstream ss;
            ss << "Brand: " << cpu.get_cpu_info(); lp.push(ss.str());
        }
        {
            std::ostringstream ss;
            ss << "Utilization: " << cpu.get_cpu_utilization() << "%"; lp.push(ss.str());
        }
        {
            std::ostringstream ss;
            ss << "Speed: " << cpu.get_cpu_speed(); lp.push(ss.str());
        }
        {
            std::ostringstream ss;
            ss << "Base Speed: " << cpu.get_cpu_base_speed(); lp.push(ss.str());
        }
        {
            std::ostringstream ss;
            ss << "Cores: " << cpu.get_cpu_cores(); lp.push(ss.str());
        }
        {
            std::ostringstream ss;
            ss << "Logical Processors: " << cpu.get_cpu_logical_processors(); lp.push(ss.str());
        }
        {
            std::ostringstream ss;
            ss << "Sockets: " << cpu.get_cpu_sockets(); lp.push(ss.str());
        }
        {
            std::ostringstream ss;
            ss << "Virtualization: " << cpu.get_cpu_virtualization(); lp.push(ss.str());
        }
        {
            std::ostringstream ss;
            ss << "L1 Cache: " << cpu.get_cpu_l1_cache(); lp.push(ss.str());
        }
        {
            std::ostringstream ss;
            ss << "L2 Cache: " << cpu.get_cpu_l2_cache(); lp.push(ss.str());
        }
        {
            std::ostringstream ss;
            ss << "L3 Cache: " << cpu.get_cpu_l3_cache(); lp.push(ss.str());
        }
    }

    // GPU Info (detailed)
    {
        cout << endl;
        auto all_gpu_info = obj_gpu.get_all_gpu_info();
        if (all_gpu_info.empty()) {
            lp.push("--- GPU Info ---");
            lp.push("No GPU detected.");
        }
        else {
            lp.push("--- GPU Info ---");
            for (size_t i = 0; i < all_gpu_info.size(); ++i) {
                auto& g = all_gpu_info[i];
                {
                    std::ostringstream ss;
                    ss << "GPU " << (i + 1) << ":"; lp.push(ss.str());
                }
                {
                    std::ostringstream ss;
                    ss << "  Name: " << g.gpu_name; lp.push(ss.str());
                }
                {
                    std::ostringstream ss;
                    ss << "  Memory: " << g.gpu_memory; lp.push(ss.str());
                }
                {
                    std::ostringstream ss;
                    ss << "  Usage: " << g.gpu_usage << "%"; lp.push(ss.str());
                }
                {
                    std::ostringstream ss;
                    ss << "  Vendor: " << g.gpu_vendor; lp.push(ss.str());
                }
                {
                    std::ostringstream ss;
                    ss << "  Driver Version: " << g.gpu_driver_version; lp.push(ss.str());
                }
                {
                    std::ostringstream ss;
                    ss << "  Temperature: " << g.gpu_temperature << " C"; lp.push(ss.str());
                }
                {
                    std::ostringstream ss;
                    ss << "  Core Count: " << g.gpu_core_count; lp.push(ss.str());
                }
            }

            auto primary = detailed_gpu_info.primary_gpu_info();
            {
                std::ostringstream ss;
                ss << "Primary GPU Details:"; lp.push(ss.str());
            }
            {
                std::ostringstream ss;
                ss << "  Name: " << primary.name; lp.push(ss.str());
            }
            {
                std::ostringstream ss;
                ss << "  VRAM: " << primary.vram_gb << " GiB"; lp.push(ss.str());
            }
            {
                std::ostringstream ss;
                ss << "  Frequency: " << primary.frequency_ghz << " GHz"; lp.push(ss.str());
            }
        }
    }

    // Display Info
    {
        cout << endl;
        lp.push("--- Display Info ---");
        auto monitors = display.get_all_displays();
        if (monitors.empty()) {
            lp.push("No monitors detected.");
        }
        else {
            for (size_t i = 0; i < monitors.size(); ++i) {
                auto& m = monitors[i];
                {
                    std::ostringstream ss;
                    ss << "Monitor " << (i + 1) << ":"; lp.push(ss.str());
                }
                {
                    std::ostringstream ss;
                    ss << "  Brand: " << m.brand_name; lp.push(ss.str());
                }
                {
                    std::ostringstream ss;
                    ss << "  Resolution: " << m.resolution; lp.push(ss.str());
                }
                {
                    std::ostringstream ss;
                    ss << "  Refresh Rate: " << m.refresh_rate << " Hz"; lp.push(ss.str());
                }
            }
        }
    }

    // BIOS & Motherboard Info
    {
        cout << endl;
        lp.push("--- BIOS & Motherboard Info ---");
        {
            std::ostringstream ss;
            ss << "Bios Vendor: " << sys.get_bios_vendor(); lp.push(ss.str());
        }
        {
            std::ostringstream ss;
            ss << "Bios Version: " << sys.get_bios_version(); lp.push(ss.str());
        }
        {
            std::ostringstream ss;
            ss << "Bios Date: " << sys.get_bios_date(); lp.push(ss.str());
        }
        {
            std::ostringstream ss;
            ss << "Motherboard Model: " << sys.get_motherboard_model(); lp.push(ss.str());
        }
        {
            std::ostringstream ss;
            ss << "Motherboard Manufacturer: " << sys.get_motherboard_manufacturer(); lp.push(ss.str());
        }
    }

    // User Info
    {
        cout << endl;
        lp.push("--- User Info ---");
        {
            std::ostringstream ss;
            ss << "Username: " << user.get_username(); lp.push(ss.str());
        }
        {
            std::ostringstream ss;
            ss << "Computer Name: " << user.get_computer_name(); lp.push(ss.str());
        }
        {
            std::ostringstream ss;
            ss << "Domain: " << user.get_domain_name(); lp.push(ss.str());
        }
        {
            std::ostringstream ss;
            ss << "Groups: " << user.get_user_groups(); lp.push(ss.str());
        }
    }

    // Performance Info
    {
        cout << endl;
        lp.push("--- Performance Info ---");
        {
            std::ostringstream ss;
            ss << "System Uptime: " << perf.get_system_uptime(); lp.push(ss.str());
        }
        {
            std::ostringstream ss;
            ss << "CPU Usage: " << perf.get_cpu_usage_percent() << "%"; lp.push(ss.str());
        }
        {
            std::ostringstream ss;
            ss << "RAM Usage: " << perf.get_ram_usage_percent() << "%"; lp.push(ss.str());
        }
        {
            std::ostringstream ss;
            ss << "Disk Usage: " << perf.get_disk_usage_percent() << "%"; lp.push(ss.str());
        }
        {
            std::ostringstream ss;
            ss << "GPU Usage: " << perf.get_gpu_usage_percent() << "%"; lp.push(ss.str());
        }
    }

    // ---------------- End of info lines ----------------

    // Print remaining ASCII art lines (if art is taller than info)
    lp.finish();

    std::cout << std::endl;
    return 0;
}


/*

PROJECT: BinaryFetch (main.cpp)

PURPOSE:
Collects and displays both compact and detailed system information
side-by-side with ASCII art using a LivePrinter streaming system.

===============================================================================
USED CLASSES, OBJECTS, AND FUNCTIONS

==================== ASCII / OUTPUT SYSTEM ====================

Class: AsciiArt
Object:
AsciiArt art;

Member Functions:
bool loadFromFile(const std::string& filePath)
- Loads ASCII art from a text file.
- Returns true if load succeeds, false otherwise.

Class: LivePrinter
Object:
LivePrinter lp(art);

Member Functions:
void push(const std::string& line)
- Streams one line of output aligned with ASCII art.

void finish()
    - Outputs remaining ASCII art lines when info lines end.

===============================================================================
COMPACT MODE CLASSES (LIGHTWEIGHT SUMMARY)

Class: CompactOS
Object:
CompactOS c_os;

Functions:
std::string getOSName()
std::string getOSBuild()
std::string getArchitecture()
std::string getUptime()

Class: CompactCPU
Object:
CompactCPU c_cpu;

Functions:
std::string getCPUName()
int getCPUCores()
int getCPUThreads()
double getClockSpeed()

Class: CompactScreen
Object:
CompactScreen c_screen;

Functions:
std::vector<ScreenInfo> get_screens()

Struct: ScreenInfo
std::string brand_name
std::string resolution
int refresh_rate

Class: CompactMemory
Object:
CompactMemory c_memory;

Functions:
double get_total_memory()
double get_free_memory()
double get_used_memory_percent()

Class: CompactAudio
Object:
CompactAudio c_audio;

Functions:
std::string active_audio_input()
std::string active_audio_input_status()
std::string active_audio_output()
std::string active_audio_output_status()

Class: CompactSystem
Object:
CompactSystem c_system;

Functions:
std::string getBIOSInfo()
std::string getMotherboardInfo()

Class: CompactGPU
Object:
CompactGPU c_gpu;

Functions:
std::string getGPUName()
float getGPUUsagePercent()
float getVRAMGB()
std::string getGPUFrequency()

Class: CompactPerformance
Object:
CompactPerformance c_perf;

Functions:
float getCPUUsage()
float getGPUUsage()
float getRAMUsage()
float getDiskUsage()

Class: CompactUser
Object:
CompactUser c_user;

Functions:
std::string getUsername()
std::string getDomain()
std::string isAdmin()

Class: CompactNetwork
Object:
CompactNetwork c_net;

Functions:
std::string get_network_name()
std::string get_network_type()
std::string get_network_ip()

Class: DiskInfo
Object:
DiskInfo disk;

Functions:
std::vector<std::pairstd::string,double
> getAllDiskUsage()
std::vector<std::pairstd::string,int
> getDiskCapacity()

===============================================================================
FULL DETAIL SYSTEM CLASSES

Class: OSInfo
Object:
OSInfo os;

Functions:
std::string GetOSName()
std::string GetOSVersion()
std::string GetOSArchitecture()
std::string get_os_kernel_info()
std::string get_os_uptime()
std::string get_os_install_date()
std::string get_os_serial_number()

Class: CPUInfo
Object:
CPUInfo cpu;

Functions:
std::string get_cpu_info()
float get_cpu_utilization()
std::string get_cpu_speed()
std::string get_cpu_base_speed()
int get_cpu_cores()
int get_cpu_logical_processors()
int get_cpu_sockets()
std::string get_cpu_virtualization()
std::string get_cpu_l1_cache()
std::string get_cpu_l2_cache()
std::string get_cpu_l3_cache()

Class: MemoryInfo
Object:
MemoryInfo ram;

Functions:
double getTotal()
double getFree()
float getUsedPercentage()
std::vector<MemoryModule> getModules()

Struct: MemoryModule
std::string capacity
std::string type
std::string speed

Class: GPUInfo
Object:
GPUInfo obj_gpu;

Functions:
std::vector<GPUData> get_all_gpu_info()

Struct: GPUData
std::string gpu_name
std::string gpu_memory
int gpu_usage
std::string gpu_vendor
std::string gpu_driver_version
int gpu_temperature
int gpu_core_count

Class: DetailedGPUInfo
Object:
DetailedGPUInfo detailed_gpu_info;

Functions:
GPUDetail primary_gpu_info()

Struct: GPUDetail
std::string name
float vram_gb
float frequency_ghz

Class: StorageInfo
Object:
StorageInfo storage;

Functions:
std::vector<StorageData> get_all_storage_info()

Struct: StorageData
std::string storage_type
std::string drive_letter
std::string used_space
std::string total_space
std::string used_percentage
std::string file_system
bool is_external
std::string read_speed
std::string write_speed
std::string predicted_read_speed
std::string predicted_write_speed
std::string serial_number

Class: DisplayInfo
Object:
DisplayInfo display;

Functions:
std::vector<MonitorInfo> get_all_displays()

Struct: MonitorInfo
std::string brand_name
std::string resolution
int refresh_rate

Class: SystemInfo
Object:
SystemInfo sys;

Functions:
std::string get_bios_vendor()
std::string get_bios_version()
std::string get_bios_date()
std::string get_motherboard_model()
std::string get_motherboard_manufacturer()

Class: UserInfo
Object:
UserInfo user;

Functions:
std::string get_username()
std::string get_computer_name()
std::string get_domain_name()
std::string get_user_groups()

Class: PerformanceInfo
Object:
PerformanceInfo perf;

Functions:
std::string get_system_uptime()
float get_cpu_usage_percent()
float get_ram_usage_percent()
float get_disk_usage_percent()
float get_gpu_usage_percent()

Class: ExtraInfo
Object:
ExtraInfo extra;

Functions:
void get_audio_devices()
void get_power_status()

===============================================================================
END OF DOCUMENTATION

*/
