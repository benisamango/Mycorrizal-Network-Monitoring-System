# Mycorrizal-Network-Monitoring-System
This repository contains all the code used within my thesis project on building a Mycorrizal Network Monitoring System.
The goal of this project is to explore the bioelectrical responses of fungi, particularly white oyster mushrooms (*Pleurotus ostreatus*), to  environmental stimuli such as moisture. This research investigates whether fungi exhibit patterns in their electrical responses that could suggest communication within their network.

## Project Overview

The project is conducted using a combination of hardware and software, including:
- **M5Core2 Microcontroller** for data acquisition and processing
- **24-bit ADC (SPARKX QWIIC)** for capturing high-resolution voltage signals
- **Electrodes** for extracellular measurement of fungal electrical activity
- **SD Card and Wi-Fi Logging** for storing and transmitting data
- **Custom Scripts** in Python and Arduino for data collection, processing, and visualization

This repository includes the code for data collection, data processing, and analysis.

## Folder Structure

- `Arduino_Code/`: Contains the code for the M5Core2 to capture and log electrical responses from the fungi. This includes interfacing with the ADC and saving data to the SD card.
- `Python_Scripts/`: Contains data processing and analysis scripts used to clean, filter, and analyze the recorded electrical data. These scripts generate visualizations of the voltage response over time.
- `Data/`: Folder for storing raw data collected from the M5Core2 (sample files included for testing and analysis).
- `Visualizations/`: Scripts and examples for generating visual representations of the data, including time-series graphs of the fungi's electrical response to stimuli.

## Code Overview

### Arduino Code (Data Acquisition)

The Arduino code in `Arduino_Code/` initializes the ADC and connects to the M5Core2's components. It collects real-time voltage data from the electrodes then logs it to the SD card. The data is timestamped, and Wi-Fi functionality allows the device to send data to an external server for remote monitoring.

### Python Code (Data Processing and Analysis)

The Python scripts in `Python_Scripts/` are used for:
- **Visualization**: Creating time-series plots to analyze voltage spikes and patterns in response to stimuli.

### Visualizations

The `Visualizations/` folder contains scripts and examples for generating figures and graphs used in the thesis. These visualizations are essential for identifying patterns in the fungi's response to environmental changes.

## Requirements

- **Arduino IDE**: To upload code to the M5Core2.
- **Python 3.x**: For running the data processing and visualization scripts.
- **Required Libraries**:
  - Arduino: `M5Stack`, `Wire`, `WiFi`, `SD`
  - Python: `pandas`, `matplotlib`, `scipy`, `numpy`

## Usage

1. **Set up the M5Core2** with electrodes and ADC system.
2. **Upload the Arduino code** from `Arduino_Code/` to the M5Core2 for data acquisition.
3. **Collect data** and transfer it from the SD card to the `Data/` folder.
4. **Run the Python scripts** in `Python_Scripts/` for data analysis and visualization.

## Acknowledgments

The internet dashboard used within this project is designed by Olivia Ronda. 
This project was supported with guidance and insights provided by **ChatGPT**.


