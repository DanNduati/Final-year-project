<h1 align="center"><b>Final Year Project: Development of a industrial IoT application with web based digital twin
</b></h1>

<h1 align="center">Case study: Elevator </h1>

## Project Description
This is a repository containing the documentation and files of the design,implementation and testing of the IIoT system by group 22. IIoT is one of the industry 4.0 technology. Our project is about connecting industrial devices to the cloud in this case we fimlly went for Google cloud platform where we use the Firebase Realtime Database a cloud-hosted database to store elevator data as JSON and synchronized in Realtime to every connected client. Using Database trigger event handlers we handle on-change events of the data in the database and control and simulate floor changes on a web hosted digital twin model of the elevator.

# Preview
<p align="center">
<img height="500" src="images/elevator/elevator.gif" alt="animated" />
<img height="500" src="images/elevator/elevator.png"/>
</p>

# Table of contents
* [Group Members](#Group-Members)
* [Directory Structure](#Directory-Structure)
* [Project Objectives](#Project-Objectives)
* [System Architecture](#System-Architecture)
* [Project Status](#Status)
* [License](#License)

# Group-Members
1. Nduati Daniel Chege
2. Mwangi Maxwell Wachira
3. Ian Wanjir Owuor 
4. Amos Mosoito Mutete 

# Directory-Structure
    .
    ├── Budget			    # Project budget
    ├── docs			    # Project documentation and reference material
	├── firmware			# Firmware files
			├── Rpi			# Raspberry pi firmware
			├── Esp32   	# ESP32 c/c++ firmware files
	├── images              # Image files
    ├── iot2040		    	# IoT2040 commisioning documentation
    ├── onboarding			# Mindsphere rpi onboarding documentation
    ├── res     			# welp
    ├── threejs 			# Elevator threejs basic boilerplate
    ├── twin     			# Elevator threejs webpack boilerplate 
	└── README.md

# Project-Objectives
1. Connect the  elevator model to the cloud.
2. Create a cloud based digital twin of the elevator system that simulates the actual elevator system real time.
3. Integrating the Individual systems so that they function together seamlessly.
4. Collect, monitor and visualize the system data real time on a cloud IOT platform.

# System-Architecture

# Status
[Progress Board](https://github.com/DanNduati/Final-Year-Project/projects/1)

# License
This project is licensed under the terms of the [MIT license](LICENSE).