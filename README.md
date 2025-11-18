# MPU6050-3D-Visualization
MATLAB Simulink project for real-time 3D visualization of MPU6050 orientation data using ESP32 and VR Sink.


## Requirements
- **Hardware**  
  - ESP32 development board  
  - MPU6050 IMU module  
  - Optional additional sensors (DHT22, BMP280)  
- **Software**  
  - MATLAB  
  - Simulink  
  - Instrument Control Toolbox (for UDP functionality, depending on setup)  
  - Simulink 3D Animation (for VR Sink and 3D visualization)
 
    
## Repository Structure
- `Arduino_Code/`  
  ESP32 code to read MPU6050 and send data via UDP.
- `Simulink_Model/`  
  Simulink files (`.slx`) and MATLAB functions used for:
  - Complementary filter  
  - Converting sensor data to VR Sink format  
- `assets/`  
  3D model files (for example, `.stl`) used by the VR Sink block for visualization.


## Hardware: ESP32 ↔ MPU6050 Connections

The ESP32 and MPU6050 communicate over **I²C**. A typical wiring is:
  **MPU6050**       **ESP32**
- **VCC**       →   **3.3V**  
- **GND**       →   **GND**  
- **SDA**       →   **GPIO 21 (SDA)**  
- **SCL**       →   **GPIO 22 (SCL)**  
The sensor should be powered at **3.3V**. 


## How the System Works? (Overview)
1. The **MPU6050** measures motion and orientation information in three axes using:
   - An accelerometer  
   - A gyroscope  
2. The ESP32 reads this data over I²C and prepares it for transmission.
3. The ESP32 sends the sensor data to the computer using UDP packets over Wi-Fi.
4. Simulink receives the UDP data using a UDP Receive block and unpacks it into meaningful physical values.
5. A complementary filter combines accelerometer and gyroscope data to estimate smooth and stable euler angles.
6. The roll & pitch (in radians) from the MPU6050 is remaped to match the VR coordinate system, a quaternion using a fixed yaw of zero is built , which is again converted into an axis–  angle representation and is returned in the format that VR Sink expects.
7. The VR Sink uses a 3D model (for example, an `.stl` file) to visually show the real-time orientation of the sensor in a virtual scene.


## What the MPU6050 Measures?
The MPU6050 is a 6-axis IMU (Inertial Measurement Unit). It contains:
- A **3-axis accelerometer**  
- A **3-axis gyroscope**

### Accelerometer: ax, ay, az
- `ax`, `ay`, `az` represent accelerations along the sensor’s **X, Y, Z axes**.  
- When the sensor is stationary, these values mainly represent **gravity**, split between the three axes depending on orientation.  
- These values can be expressed in units like **g** (acceleration due to gravity) or **m/s²**, depending on conversion.

### Gyroscope: gx, gy, gz
- `gx`, `gy`, `gz` represent the angular velocity (how fast the sensor is rotating) around the X, Y, and Z axes.  
- They are typically expressed in **degrees per second** or **radians per second**.  
- Gyroscope data is useful for tracking fast changes in orientation, but it gradually drifts over time if used alone.
Together, these six values describe how the sensor is moving and how it is oriented in space.


## UDP Packets: How the Data is Sent?
The ESP32 sends sensor data to the computer using **UDP** (User Datagram Protocol).
Key points about the UDP communication:
- A fixed packet structure is used so that both ESP32 and Simulink agree on the order and type of data (for example: `ax, ay, az, gx, gy, gz` in a fixed order).  
- UDP is connectionless, which means:
  - It is simple and fast.  
  - Packets may occasionally be lost, but this is usually acceptable for real time visualization.  
- Simulink’s UDP Receive block listens on a chosen port number, which must match the port configured in the ESP32 code.
In Simulink, the received bytes are unpacked (for example, using a byte unpack block) into the original numerical values for `ax, ay, az, gx, gy, gz`.


## Why Use Euler Angles?
Raw sensor values (`ax, ay, az, gx, gy, gz`) are not very intuitive to visualize directly.  
To describe orientation in a human friendly way, we often use Euler angles:
- **Roll**: rotation around the X axis   
- **Pitch**: rotation around the Y axis  
- **Yaw**: rotation around the Z axis 

In this project:
- Roll and pitch are estimated using accelerometer and gyroscope data.  
- Yaw is typically not reliable without a magnetometer, so it may be ignored or left as zero in basic examples.
Euler angles are convenient because:
- They clearly show how the object is tilted.  
- They can be directly used to rotate 3D models in tools like VR Sink.


## Complementary Filter: Combining Accelerometer and Gyroscope
Both sensors have strengths and weaknesses:
- **Accelerometer**  
  - Good for long-term stability because gravity is a constant reference.  
  - Noisy during fast motions or vibrations.  
- **Gyroscope**  
  - Good at capturing quick rotational movements with smooth signals.  
  - Suffers from drift over time when integrated to get angles.
A **complementary filter** combines these two:
- It trusts the gyroscope for short-term changes.  
- It trusts the accelerometer for long-term stability and correction.
Conceptually, at each time step:
- The angle estimate is updated using gyroscope data.  
- The accelerometer-based angle is used to slowly correct this estimate.  
- A tuning parameter controls how much weight is given to each sensor.
The result is a smoother and more stable estimate of roll and pitch compared to using either sensor alone.


## Working with the Simulink Model and VR Sink
The Simulink model usually contains:
- A UDP Receive block to get data from the ESP32.  
- Logic or blocks to unpack the incoming data into `ax, ay, az, gx, gy, gz`.  
- A complementary filter implementation to calculate roll and pitch.  
- Conversion from roll and pitch to the format expected by VR Sink.  
- A VR Sink block linked to a 3D model (for example, an `.stl` file representing a board or object).
The 3D model rotates in real time according to the estimated orientation, giving a clear visual of how the physical MPU6050 setup is moving.


## How to Run the Project?
1. Upload the ESP32 code (.ino file) to your ESP32 board.  
   Configure your Wi-Fi credentials and UDP IP/port inside the sketch if required.
2. Open the Simulink model:  
3. Make sure the .stl folder is:
   - In the same project or  
   - On the MATLAB path  
4. In the Simulink model, check that:
   - The UDP Receive block’s local port matches the UDP port set in the ESP32 code.  
   - The unpacking logic matches the packet format sent by the ESP32.  
5. Run the model and open the VR Sink viewer to see the 3D scene.  
   You should see the virtual model move in sync with the physical MPU6050 orientation.

