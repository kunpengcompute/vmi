# Security Statement

Please carefully read the following content to ensure that the security risks of the solution are acceptable.

1. The communication between the device and the cloud depends on a third-party transmission pipe. The pipe bidirectionally transmits the data of the instruction stream engine, audio engine, touch engine, and video stream engine. Therefore, the transmission pipe may include but is not limited to the following functions:
    1. User access authentication: When a device connects to the cloud, multi-factor authentication is performed to verify the user identity authenticity, which may connect to the account system of the service operator.
    2. Transmission encryption: The transmission pipe uses secure encryption algorithms to encrypt and transmit data. The impact on communication latency and bandwidth must be controllable.
    3. Reliable transmission: No packet loss, disorder, error, or duplicate packet occurs during transmission. In addition, DDoS attacks can be prevented.
2. By default, the cloud-side binary deliverables provided by the device-cloud synergy engine are preset in the Android system directory. When integrating solutions, you should comply with the permission setting requirements of the Android system directory. That is, minimize the file owners of and permissions on the device-cloud synergy engine deliverables to prevent data tampering and security risks.
3. APKs running on cloud phones are installed and used by cloud phone users. It is recommended that cloud phone users download legal apps from mainstream app stores or other legal channels to prevent malicious apps from intruding into cloud phones and causing information leakage.
4. The device-cloud synergy engine runs on Android and is started as an independent process or loaded by service processes as a dynamic link library. The integrated solutions must be compatible with the secure startup mechanism of the device-cloud synergy engine to ensure that the correct engine version is loaded when the Android system or a service process is started, preventing security risks caused by forged or counterfeit engine libraries.
5. The device-side binary deliverables of the device-cloud synergy engine provide base engine capabilities and can be integrated by third parties. The design of third-party integrated APKs must comply with the security requirements. End-to-end solutions, such as secure access, authentication, and authorization, must be provided to prevent personal information leakage. In addition, key operations of applications, including but not limited to login, logout, screenshot, and screen recording, must be recorded in logs to facilitate operation backtracking.

## Change History

|Document Version|Date|Description|
|--|--|--|
|01|2026-09-30|This is the first official release.|
